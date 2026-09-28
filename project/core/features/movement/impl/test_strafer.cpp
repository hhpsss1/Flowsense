#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>

namespace features::movement {

	// Порт проверенного subtick-стрейфера (32 сабтика, yaw-дельты, W-only мувы,
	// acos-идеал + bang-bang выбор стороны, по-сабтиковый air_move сим),
	// адаптированный под наш пайплайн:
	//  - базис цели: реальная камера (снапшот антиаима), т.к. антиаим бежит
	//    раньше и base->viewangles уже фейк; целиться от фейка = стрейф мимо;
	//  - бюджет: 32 - 2 (резерв под 2 jump-степа бхопа, он бежит следом);
	//  - max_speed: min(m_flMaxspeed, sv_maxspeed, weapon vdata) + stamina^2.

	namespace {

		constexpr auto k_subtick_cap{ 32 };
		constexpr auto k_jump_reserve{ 2 };
		constexpr auto k_min_strafe_speed{ 5.0f };

		[[nodiscard]] float resolve_max_speed( std::uintptr_t local_pawn, float stamina )
		{
			auto max_speed = CONVAR ("sv_maxspeed")->get<float>( );

			if ( local_pawn )
			{
				if ( const auto movement_services = memory::read<std::uintptr_t>( local_pawn + SCHEMA( "C_BasePlayerPawn", "m_pMovementServices"_hash ) ) )
				{
					const auto cmd_maxspeed = memory::read<float>( movement_services + SCHEMA( "CPlayer_MovementServices", "m_flMaxspeed"_hash ) );
					if ( std::isfinite( cmd_maxspeed ) && cmd_maxspeed > 0.0f )
					{
						max_speed = std::fminf( max_speed, cmd_maxspeed );
					}
				}
			}

			// Ограничение скоростью оружия, как в airstrafe::rotate_to_stop.
			const auto& ctx = features::combat::g_shared.ctx( );
			if ( ctx.valid && ctx.weapon_vdata )
			{
				const auto weapon_speed = memory::read<float>( ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flMaxSpeed"_hash ) );
				if ( std::isfinite( weapon_speed ) && weapon_speed > 0.0f )
				{
					max_speed = std::fminf( max_speed, weapon_speed );
				}
			}

			if ( !std::isfinite( max_speed ) || max_speed <= 0.0f )
			{
				return 0.0f;
			}

			// check_parametrs из референса: стамина режет max_speed квадратично.
			if ( std::isfinite( stamina ) && stamina > 0.0f )
			{
				const auto scale = std::clamp( 1.0f - stamina / 100.0f, 0.0f, 1.0f );
				max_speed *= scale * scale;
			}

			return max_speed;
		}

		// Один шаг air_accelerate из референса. Там applied идёт в velocity
		// сразу, а остаток - через frame_velocity_delta в конце air_move; сумма
		// за сабстеп всегда min(accelspeed, addspeed) - её и применяем.
		void sim_air_accelerate( float& vel_x, float& vel_y, float wish_yaw,
			float max_speed, float air_accel, float air_max_wishspeed, float friction, float frame_time )
		{
			const auto yaw_rad = wish_yaw * ( std::numbers::pi_v<float> / 180.0f );
			auto wish_x = std::cosf( yaw_rad );
			auto wish_y = std::sinf( yaw_rad );

			// fmove = max_speed, smove = 0 => wish_speed = max_speed.
			const auto wishspd = std::fminf( max_speed, air_max_wishspeed );
			const auto addspeed = wishspd - ( vel_x * wish_x + vel_y * wish_y );
			if ( addspeed <= 0.0f )
			{
				return;
			}

			const auto accelspeed = max_speed * air_accel * friction * frame_time;
			const auto gain = std::fminf( accelspeed, addspeed );

			vel_x += wish_x * gain;
			vel_y += wish_y * gain;
		}

		[[nodiscard]] std::optional<float> strafe_yaw_for_velocity( float vel_x, float vel_y,
			float target_yaw, float max_speed, float air_accel, float air_max_wishspeed,
			float friction, float frame_time )
		{
			const auto speed = std::sqrtf( vel_x * vel_x + vel_y * vel_y );
			if ( !( speed > 0.0f ) || !( max_speed > 0.0f ) )
			{
				return std::nullopt;
			}

			auto velocity_angle = std::atan2f( vel_y, vel_x ) * ( 180.0f / std::numbers::pi_v<float> );
			math::helpers::normalize_angle( velocity_angle );

			const auto wish_cap = std::fminf( air_max_wishspeed, max_speed );
			const auto air_gain = air_accel * max_speed * frame_time * friction;

			const auto cos_ideal = std::clamp( ( wish_cap - air_gain ) / speed, -1.0f, 1.0f );
			const auto ideal = std::fminf( std::acosf( cos_ideal ) * ( 180.0f / std::numbers::pi_v<float> ), 90.0f );

			auto velocity_delta = target_yaw - velocity_angle;
			math::helpers::normalize_angle( velocity_delta );

			// Bang-bang без персистентного сайда: сторона выбирается знаком
			// дельты каждый сабстеп => детерминировано между реплеями команды.
			auto result = ( ( std::fabsf( velocity_delta ) > 170.0f && speed > 80.0f ) || velocity_delta > 0.0f )
				? velocity_angle + ideal
				: velocity_angle - ideal;
			math::helpers::normalize_angle( result );
			return result;
		}

	} // namespace

	[[nodiscard]] bool test_strafer::is_active( ) const
	{
		if ( !settings::g_movement.m_test_strafer.enabled.value )
		{
			return false;
		}

		if ( !CONVAR ("sv_quantize_movement_input")->get<bool>( ) )
		{
			return false;
		}

		// Без sv_subtick_movement_view_angles yaw-дельты не стрейфят вообще -
		// спамить ими значит только жечь бюджет и ронять степы бхопа.
		if ( const auto cv = CONVAR ("sv_subtick_movement_view_angles") )
		{
			if ( !cv->get<bool>( ) )
			{
				return false;
			}
		}

		return true;
	}

	math::vector2 test_strafer::movement_from_buttons( std::uintptr_t pressed )
	{
		auto forward_move{ 0.0f };
		auto left_move{ 0.0f };

		if ( pressed & cstypes::command_buttons::in_forward )
		{
			forward_move = 1.0f;
		}
		else if ( pressed & cstypes::command_buttons::in_back )
		{
			forward_move = -1.0f;
		}

		if ( pressed & cstypes::command_buttons::in_moveleft )
		{
			left_move = -1.0f;
		}
		else if ( pressed & cstypes::command_buttons::in_moveright )
		{
			left_move = 1.0f;
		}

		return { forward_move, left_move };
	}

	void test_strafer::on_create_move( systems::input::usercmd* cmd )
	{
		this->m_handled_this_tick = false;

		if ( !this->is_active( ) )
		{
			return;
		}

		if ( features::movement::g_jumpbug.active_this_tick( ) )
		{
			return;
		}

		const auto base = cmd->csgo_user_cmd.mutable_base( );
		if ( !base )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.pawn )
		{
			return;
		}

		const auto move_type = memory::read<std::uint8_t>( local.pawn + SCHEMA( "C_BaseEntity", "m_nActualMoveType"_hash ) );
		if ( move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip )
		{
			return;
		}

		const auto& prestate = systems::g_prediction.pre( );
		if ( prestate.flags & cstypes::entity_flags::on_ground )
		{
			return;
		}

		if ( features::combat::g_rage.is_firing_this_tick( ) )
		{
			return;
		}

		this->quantized_path( cmd );
	}

	bool test_strafer::apply_yaw_subtick( proto::base_usercmd_pb* base, float when, float yaw_delta ) const
	{
		math::helpers::normalize_angle( yaw_delta );

		if ( !std::isfinite( yaw_delta ) || !std::isfinite( when ) )
		{
			return false;
		}

		when = std::clamp( when, 0.0f, 1.0f );

		const auto subtick_moves = base->mutable_subtick_moves( );
		if ( !subtick_moves )
		{
			return false;
		}

		const auto step = systems::g_input.acquire_subtick_step( subtick_moves );
		if ( !step )
		{
			return false;
		}

		step->set_when( when );
		step->set_button( 0 );
		step->set_pressed( false );
		step->set_analog_forward_delta( 0.0f );
		step->set_analog_left_delta( 0.0f );
		step->set_yaw_delta( yaw_delta );
		step->set_pitch_delta( 0.0f );
		return true;
	}

	void test_strafer::quantized_path( systems::input::usercmd* cmd )
	{
		const auto current_buttons = cmd->buttons.value;

		// Как в референсе: стрейфер - компаньон бхопа (нужен зажатый прыжок),
		// спринт отключает. Важно: читаем кнопки ДО того, как бхоп ниже по
		// пайплайну срежет in_jump - поэтому стрейфер бежит раньше бхопа.
		if ( !( current_buttons & static_cast< std::uintptr_t >( cstypes::command_buttons::in_jump ) ) )
		{
			return;
		}

		if ( current_buttons & static_cast< std::uintptr_t >( cstypes::command_buttons::in_sprint ) )
		{
			return;
		}

		const auto base = cmd->csgo_user_cmd.mutable_base( );
		if ( !base )
		{
			return;
		}

		this->check_button( current_buttons, cstypes::command_buttons::in_moveleft );
		this->check_button( current_buttons, cstypes::command_buttons::in_moveright );
		this->check_button( current_buttons, cstypes::command_buttons::in_forward );
		this->check_button( current_buttons, cstypes::command_buttons::in_back );
		this->m_last_buttons = current_buttons;

		const auto local = systems::g_local.get( );

		const auto& prestate = systems::g_prediction.pre( );
		const auto velocity = prestate.networked_velocity;
		if ( !std::isfinite( velocity.x ) || !std::isfinite( velocity.y ) )
		{
			return;
		}

		const auto speed_2d = velocity.length_2d( );
		if ( !std::isfinite( speed_2d ) || speed_2d < k_min_strafe_speed )
		{
			return;
		}

		const auto player_move = movement_from_buttons( this->m_last_pressed );
		if ( player_move.x == 0.0f && player_move.y == 0.0f )
		{
			return;
		}

		const auto sv_airaccelerate = CONVAR ("sv_airaccelerate")->get<float>( );
		const auto sv_air_max_wishspeed = CONVAR ("sv_air_max_wishspeed")->get<float>( );
		const auto surface_friction = prestate.surface_friction;

		if ( !std::isfinite( sv_airaccelerate ) || !std::isfinite( sv_air_max_wishspeed )
			|| !std::isfinite( surface_friction ) || sv_airaccelerate <= 0.0f
			|| sv_air_max_wishspeed <= 0.0f || surface_friction <= 0.0f )
		{
			return;
		}

		const auto max_speed = resolve_max_speed( local.pawn, prestate.stamina );
		if ( !( max_speed > 0.0f ) )
		{
			return;
		}

		// Бюджет с учётом уже лежащих степов + резерв под прыжок бхопа.
		const auto existing_steps = base->subtick_moves_size( );
		const auto substeps = std::clamp( k_subtick_cap - k_jump_reserve - existing_steps, 1, k_subtick_cap );
		if ( substeps <= 0 )
		{
			return;
		}

		const auto frame_time = cstypes::tick_interval / static_cast< float >( substeps );

		// W-only мувы (точно представимы под квантайзом), рулим только yaw.
		base->set_forwardmove( 1.0f );
		base->set_leftmove( 0.0f );

		// Базис цели - реальная камера (см. get_real_angles): base->viewangles
		// уже перезаписан фейком антиаима.
		const auto submitted_yaw = base->viewangles( )->y( );
		auto ref_yaw = submitted_yaw;
		const auto& aa = features::combat::g_misc.antiaim( );
		if ( aa.is_rotating_command( ) )
		{
			const auto real_yaw = aa.get_real_angles( ).y;
			if ( std::isfinite( real_yaw ) )
			{
				ref_yaw = real_yaw;
			}
		}

		const auto base_yaw_offset = std::atan2f( -player_move.y, player_move.x ) * ( 180.0f / std::numbers::pi_v<float> );
		auto target_yaw = ref_yaw + base_yaw_offset;
		math::helpers::normalize_angle( target_yaw );

		if ( !std::isfinite( target_yaw ) || !std::isfinite( submitted_yaw ) )
		{
			return;
		}

		// run_yaw продолжает уже лежащие yaw-дельты (как в референсе).
		auto run_yaw = submitted_yaw;
		for ( auto i = 0; i < existing_steps; ++i )
		{
			if ( const auto step = base->mutable_subtick_moves( i ) )
			{
				const auto dy = step->yaw_delta( );
				if ( std::isfinite( dy ) )
				{
					run_yaw += dy;
				}
			}
		}
		math::helpers::normalize_angle( run_yaw );

		const auto view_pitch = base->viewangles( )->x( );
		(void) view_pitch;

		auto sim_vx = velocity.x;
		auto sim_vy = velocity.y;
		auto injected = 0;

		for ( auto i = 1; i <= substeps; ++i )
		{
			const auto strafe_yaw = strafe_yaw_for_velocity( sim_vx, sim_vy, target_yaw,
				max_speed, sv_airaccelerate, sv_air_max_wishspeed, surface_friction, frame_time );
			if ( !strafe_yaw.has_value( ) )
			{
				break;
			}

			const auto target = *strafe_yaw;
			const auto when = static_cast< float >( i * 64 / substeps ) / 64.0f;

			auto yaw_delta = target - run_yaw;
			math::helpers::normalize_angle( yaw_delta );

			if ( !this->apply_yaw_subtick( base, when, yaw_delta ) )
			{
				break;
			}

			++injected;
			run_yaw = target;

			if ( i == substeps )
			{
				break;
			}

			sim_air_accelerate( sim_vx, sim_vy, target,
				max_speed, sv_airaccelerate, sv_air_max_wishspeed, surface_friction, frame_time );
		}

		if ( injected <= 0 )
		{
			return;
		}

		// Синхронизируем кнопки под W-only мувы (как apply_move_buttons).
		cmd->buttons.value &= ~( static_cast< std::uintptr_t >( cstypes::command_buttons::in_back )
			| static_cast< std::uintptr_t >( cstypes::command_buttons::in_moveleft )
			| static_cast< std::uintptr_t >( cstypes::command_buttons::in_moveright ) );
		cmd->buttons.value |= static_cast< std::uintptr_t >( cstypes::command_buttons::in_forward );

		this->m_handled_this_tick = true;
	}

	void test_strafer::check_button( std::uintptr_t current_buttons, std::uintptr_t button )
	{
		constexpr auto moveleft = static_cast< std::uintptr_t >( cstypes::command_buttons::in_moveleft );
		constexpr auto moveright = static_cast< std::uintptr_t >( cstypes::command_buttons::in_moveright );
		constexpr auto forward = static_cast< std::uintptr_t >( cstypes::command_buttons::in_forward );
		constexpr auto back = static_cast< std::uintptr_t >( cstypes::command_buttons::in_back );

		if ( current_buttons & button && ( !( this->m_last_buttons & button ) || ( button & moveleft && !( this->m_last_pressed & moveright ) ) || ( button & moveright && !( this->m_last_pressed & moveleft ) ) || ( button & forward && !( this->m_last_pressed & back ) ) || ( button & back && !( this->m_last_pressed & forward ) ) ) )
		{
			if ( button & moveleft )
			{
				this->m_last_pressed &= ~moveright;
			}
			else if ( button & moveright )
			{
				this->m_last_pressed &= ~moveleft;
			}
			else if ( button & forward )
			{
				this->m_last_pressed &= ~back;
			}
			else if ( button & back )
			{
				this->m_last_pressed &= ~forward;
			}

			this->m_last_pressed |= button;
		}
		else if ( !( current_buttons & button ) )
		{
			this->m_last_pressed &= ~button;
		}
	}

} // namespace features::movement
