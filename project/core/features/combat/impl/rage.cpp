#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <utilities/threadpool/threadpool.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>
namespace features::combat {

	void rage::on_create_move( systems::input::usercmd* cmd )
	{
		auto& ctx = g_shared.ctx( );
		const auto local = systems::g_local.get( );
		this->update_penetration_crosshair( local );

		if ( !ctx.valid )
		{
			this->m_revolver_cock_ticks = 0;
			return;
		}

		this->m_should_stop = false;
		this->m_firing_this_tick = false;

		if ( !settings::g_combat.m_duckpeek.enabled.value )
		{
			this->m_release_duck_for_shot = false;
			this->m_duckpeek_reduck = false;
		}

		if ( this->m_zeus_fired )
		{
			this->m_zeus_fired = false;

			if ( settings::g_combat.m_zeusbot.drop_after && !systems::g_local.is_in_deathmatch( ) )
			{
				memory::call<void>(PATTERN (patterns::engine_client_cmd), addresses::globals::source2engine_to_client, 0, "drop", 0x7ffef001 );
			}

			return;
		}

		const auto is_knife = ctx.weapon_type == cstypes::weapon_type::knife;
		const auto is_taser = ctx.weapon_type == cstypes::weapon_type::taser;

		if ( !is_knife && !is_taser && ( ctx.weapon_type < cstypes::weapon_type::pistol || ctx.weapon_type > cstypes::weapon_type::lmg ) )
		{
			return;
		}

		auto aim_ctx = this->build_context( cmd, local );

		if ( is_knife )
		{
			if ( !g_shared.can_shoot( cmd, local.controller ) )
			{
				return;
			}

			this->run_knife( cmd, aim_ctx, local );
		}
		else if ( is_taser )
		{
			if ( !g_shared.can_shoot( cmd, local.controller ) )
			{
				return;
			}

			this->run_taser( cmd, aim_ctx, local );
		}
		else if ( ctx.item_def_idx == cstypes::item_definition_index::weapon_r8_revolver )
		{







			if ( settings::g_combat.m_autos.revolver_quick.value )
			{
				this->m_revolver_cock_ticks = 0;

				if ( cmd->buttons.value & cstypes::command_buttons::in_attack )
				{
					cmd->buttons.value &= ~cstypes::command_buttons::in_attack;
					cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
					cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_attack;
					cmd->csgo_user_cmd.set_attack1_start_history_index( -1 );
				}

				if ( !g_shared.can_shoot( cmd, local.controller ) )
				{
					return;
				}

				this->run_gun( cmd, aim_ctx, local );
			}
			else
			{
				this->auto_revolver( cmd, aim_ctx, local );
			}
		}
		else
		{
			this->m_revolver_cock_ticks = 0;

			if ( !g_shared.can_shoot( cmd, local.controller ) )
			{
				return;
			}

			this->run_gun( cmd, aim_ctx, local );
		}
	}

	void rage::on_render( xdraw::draw_list& draw_list )
	{
		this->draw_penetration_crosshair( draw_list );

		const auto& config = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type );




		if ( !config.debug_multipoints.value )
		{
			return;
		}

		std::lock_guard lock( m_debug_mtx );

		for ( const auto& pt : m_debug_points )
		{
			const auto screen = systems::g_view.project( pt.position );
			if ( !systems::g_view.projection_valid( screen ) )
			{
				continue;
			}

			xdraw::color col{};
			switch ( pt.hitbox_index )
			{
			case 0:
				col = { 255, 80,  80  }; break;
			case 2: case 3:
				col = { 220, 220, 60  }; break;
			case 4: case 5: case 6:
				col = { 255, 160, 60  }; break;
			case 7: case 8: case 9: case 10: case 11: case 12:
				col = { 80,  160, 255 }; break;
			case 13: case 14: case 15: case 16: case 17: case 18:
				col = { 180, 80,  255 }; break;
			default:
				col = { 200, 200, 200 }; break;
			}

			const auto alpha  = pt.is_center ? std::uint8_t{ 255 } : std::uint8_t{ 160 };
			const auto radius = pt.is_center ? 3.5f : 2.0f;

			draw_list.circle_filled( screen.x, screen.y, radius, col.alpha( alpha ) );
		}
	}

	rage::aim_context rage::build_context( systems::input::usercmd* cmd, const systems::local::snapshot& local ) const
	{
		auto& ctx = g_shared.ctx( );
		const auto& prestate = systems::g_prediction.pre( );

		aim_context out{};
		out.velocity = prestate.velocity;
		out.spread = g_shared.get_spread( );
		out.predicted_inaccuracy = g_shared.get_inaccuracy( true );

		systems::g_prediction.simulate( cmd, local, [ & ]
			{
				g_shared.sh( ).snapshot( local.pawn, ctx.weapon_services );

				out.velocity = memory::read<math::vector3>( local.pawn + SCHEMA( "C_BaseEntity", "m_vecAbsVelocity"_hash ) );
				out.spread = g_shared.get_spread( );
				out.predicted_inaccuracy = g_shared.get_inaccuracy( true );
			} );

		ctx.spread = out.spread;
		ctx.inaccuracy = out.predicted_inaccuracy;

		out.view_angles = systems::g_input.get_view_angles( );
		out.on_ground = ( prestate.flags & cstypes::entity_flags::on_ground ) != 0;




		if ( out.on_ground && ctx.ticks_since_land >= 0 && ctx.ticks_since_land <= k_land_recover_ticks && ctx.weapon_vdata )
		{
			const auto jump_apex = memory::read<float>( ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpApex"_hash ) );
			const auto t = static_cast< float >( ctx.ticks_since_land ) / static_cast< float >( k_land_recover_ticks );
			const auto landing_inaccuracy = jump_apex * ( 1.0f - t ) * ( 1.0f - t );
			out.predicted_inaccuracy = std::max( out.predicted_inaccuracy, landing_inaccuracy );
		}

		out.is_scoped = ctx.is_scoped;
		out.weapon_max_speed = ctx.weapon_max_speed;
		out.accurate_threshold = ctx.weapon_max_speed * 0.34f;

		return out;
	}

	std::optional<rage::stop_prediction> rage::predict_stop( const aim_context& ctx, const math::vector3& current_eye, const systems::local::snapshot& local ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& prestate = systems::g_prediction.pre( );
		const auto speed = prestate.networked_velocity.length_2d( );
		const auto will_stop = ctx.on_ground && ( speed > ctx.accurate_threshold || ( ctx.is_scoped && speed > 1.0f ) );

		if ( !will_stop )
		{
			return std::nullopt;
		}

		auto sim_vel = prestate.networked_velocity;
		sim_vel.z = 0.0f;

		const auto sv_friction = CONVAR("sv_friction")->get<float>( );
		const auto sv_stopspeed = CONVAR("sv_stopspeed")->get<float>( );
		const auto sv_accelerate = CONVAR("sv_accelerate")->get<float>( );
		const auto surface_friction = prestate.surface_friction;

		const auto movement_services = memory::read<std::uintptr_t>( local.pawn + SCHEMA( "C_BasePlayerPawn", "m_pMovementServices"_hash ) );
		const auto max_move_speed = movement_services ? memory::read<float>( movement_services + SCHEMA( "CPlayer_MovementServices", "m_flMaxspeed"_hash ) ) : 250.0f;

		for ( auto i = 0; i < 15; ++i )
		{
			const auto sim_speed = sim_vel.length_2d( );
			if ( sim_speed < 1.0f )
			{
				break;
			}

			const auto control = std::fmaxf( sim_speed, sv_stopspeed );
			const auto drop = sv_friction * surface_friction * control * cstypes::tick_interval;
			auto new_speed = std::fmaxf( sim_speed - drop, 0.0f );
			auto accel = sv_accelerate;

			if ( shared_ctx.is_scoped )
			{
				const auto weapon_ratio = std::fminf( 1.0f, shared_ctx.weapon_max_speed / 250.0f );
				const auto scoped_max = std::fmaxf( 250.0f, max_move_speed ) * weapon_ratio * 0.52f;

				if ( new_speed > scoped_max - 5.0f )
				{
					const auto t = 1.0f - std::fmaxf( 0.0f, new_speed - ( scoped_max - 5.0f ) ) / std::fmaxf( 0.01f, 5.0f );
					accel *= std::clamp( t, 0.0f, 1.0f );
				}
			}

			const auto accel_speed = std::fminf( accel * shared_ctx.weapon_max_speed * surface_friction * cstypes::tick_interval, new_speed );
			new_speed = std::fmaxf( new_speed - accel_speed, 0.0f );

			if ( new_speed > 0.0f )
			{
				sim_vel *= ( new_speed / sim_speed );
			}
			else
			{
				sim_vel = {};
				break;
			}
		}

		const auto avg_vel = ( prestate.networked_velocity + sim_vel ) * 0.5f;
		const auto stop_ticks = g_shared.calculate_stop_ticks( prestate.networked_velocity, shared_ctx.weapon_max_speed, local.pawn );
		const auto stop_time = static_cast< float >( stop_ticks ) * cstypes::tick_interval;

		return stop_prediction
		{
			.eye =
			{
				current_eye.x + avg_vel.x * stop_time,
				current_eye.y + avg_vel.y * stop_time,
				current_eye.z
			},
			.inaccuracy = g_shared.get_inaccuracy_at_velocity( local.pawn, sim_vel )
		};
	}

	std::vector<rage::candidate> rage::gather_candidates( const systems::local::snapshot& local, float max_distance_sq ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto players = systems::g_entities.get_by_type( systems::entities::type::player );

		std::vector<candidate> out;
		out.reserve( players.size( ) );

		const_cast<rage*>( this )->m_extrapolated_records.clear( );
		const_cast<rage*>( this )->m_extrapolated_records.reserve( players.size( ) );

		for ( const auto& p : players )
		{
			if ( !p.ptr || p.ptr == local.controller )
			{
				continue;
			}

			if ( !memory::read<bool>( p.ptr + SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash ) ) )
			{
				continue;
			}

			const auto pawn_handle = memory::read<std::uint32_t>( p.ptr + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
			const auto pawn = systems::g_entities.lookup( pawn_handle );

			if ( !pawn || pawn == local.pawn )
			{
				continue;
			}

			const auto team = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
			if ( !local.is_this_other_team( team ) )
			{
				continue;
			}

			const auto health = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
			if ( health <= 0 )
			{
				continue;
			}

			if ( memory::read<bool>( pawn + SCHEMA( "C_CSPlayerPawn", "m_bGunGameImmunity"_hash ) ) )
			{
				continue;
			}

			auto records = g_shared.lc( ).get_valid_records( pawn );

			if ( records.empty( ) )
			{
				auto extrap = g_shared.lc( ).extrapolate( pawn );
				if ( !extrap.has_value( ) )
				{


					shared::lagcomp::record live{};
					if ( !live.setup( pawn ) )
					{
						continue;
					}

					const_cast<rage*>( this )->m_extrapolated_records.push_back( std::move( live ) );
					records.push_back( &const_cast<rage*>( this )->m_extrapolated_records.back( ) );
				}
				else
				{
					const_cast<rage*>( this )->m_extrapolated_records.push_back( std::move( *extrap ) );
					records.push_back( &const_cast<rage*>( this )->m_extrapolated_records.back( ) );
				}
			}






			{
				const auto& fov_cfg = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
				const auto& pre = systems::g_prediction.pre( );
				const auto& newest_origin = records.front( )->origin;
				const auto dx = newest_origin.x - pre.origin.x;
				const auto dy = newest_origin.y - pre.origin.y;
				const auto dz = newest_origin.z - pre.origin.z;
				const auto dist_sq = dx * dx + dy * dy + dz * dz;

				if ( dist_sq > 40000.0f )
				{
					const auto view = systems::g_input.get_view_angles( );
					const auto aim = math::helpers::calculate_angle( pre.origin, newest_origin );
					if ( math::helpers::angle_distance( view, aim ) > fov_cfg.max_fov + 25.0f )
					{
						continue;
					}
				}
			}

			if ( max_distance_sq > 0.0f )
			{
				const auto& origin = systems::g_prediction.pre( ).origin;
				const auto delta_front = records.front( )->origin - origin;
				auto closest_sq = delta_front.x * delta_front.x + delta_front.y * delta_front.y + delta_front.z * delta_front.z;

				if ( records.size( ) > 1 )
				{
					const auto delta_back = records.back( )->origin - origin;
					const auto back_sq = delta_back.x * delta_back.x + delta_back.y * delta_back.y + delta_back.z * delta_back.z;
					closest_sq = std::min( closest_sq, back_sq );
				}

				if ( closest_sq > max_distance_sq )
				{
					continue;
				}
			}

			candidate c{};
			c.pawn = pawn;
			c.health = health;
			c.armor = memory::read<int>( pawn + SCHEMA( "C_CSPlayerPawn", "m_ArmorValue"_hash ) );



			if ( records.size( ) > 1 )
			{
				const auto dt_ticks = std::max( 1, records[ 0 ]->tick - records[ 1 ]->tick );
				c.speed = ( records[ 0 ]->origin - records[ 1 ]->origin ).length( ) / static_cast< float >( dt_ticks ) * 64.0f;
			}

			const auto pick_record_indices = [ &records, &c ]( std::array<int, k_max_scan_records>& out_indices ) -> int
				{
					const auto count = records.size( );
					if ( count == 0 )
					{
						return 0;
					}

					auto picked{ 0 };
					const auto add_index = [ & ]( int idx )
						{
							if ( picked >= k_max_scan_records )
							{
								return;
							}

							for ( auto i = 0; i < picked; ++i )
							{
								if ( out_indices[ i ] == idx )
								{
									return;
								}
							}

							out_indices[ picked++ ] = idx;
						};

				add_index( 0 );

				if ( count > 1 )
				{


					if ( c.speed > k_peek_speed_threshold )
					{
						add_index( 1 );
						return picked;
					}

					add_index( static_cast< int >( count - 1 ) );
				}



				const auto want = std::min<std::size_t>( count, static_cast<std::size_t>( k_max_scan_records ) );
				for ( std::size_t i = 1; i + 1 < want; ++i )
				{
					add_index( static_cast< int >( ( i * ( count - 1 ) ) / ( want - 1 ) ) );
				}

				return picked;
				};

			std::array<int, k_max_scan_records> record_indices{};
			const auto picked_count = pick_record_indices( record_indices );

			for ( auto i = 0; i < picked_count; ++i )
			{
				c.records[ i ] = records[ static_cast< std::size_t >( record_indices[ i ] ) ];
			}

			c.record_count = picked_count;

			if ( shared_ctx.weapon_type >= cstypes::weapon_type::pistol && shared_ctx.weapon_type <= cstypes::weapon_type::lmg )
			{
				const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
				c.min_damage = this->get_min_damage( config, health, config.min_damage_override.value );
			}

			out.push_back( c );
		}

		return out;
	}

	void rage::update_miss_tracking( )
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.adaptive_hitchance.value || this->m_last_shot_pawn == 0 )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.controller )
		{
			return;
		}


		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		if ( tick_base - this->m_last_shot_tick_when < 2 )
		{
			return;
		}

		const auto hp = memory::read<int>( this->m_last_shot_pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
		const auto damage_dealt = this->m_last_shot_health - hp;

		if ( damage_dealt >= 1 )
		{
			this->m_adaptive_hc_boost = 0.0f;
			this->m_miss_streak = 0;
		}
		else
		{
			++this->m_miss_streak;
			if ( this->m_miss_streak >= config.adaptive_hitchance_misses.value )
			{
				this->m_adaptive_hc_boost = std::min( this->m_adaptive_hc_boost + config.adaptive_hitchance_boost.value, 0.25f );
				this->m_miss_streak = 0;
			}
		}

		this->m_last_shot_pawn = 0;
		this->m_last_shot_health = 0;
	}

	rage::timing_result rage::evaluate_shot_timing( const aim_context& ctx, std::vector<candidate>& candidates, const target& best, const systems::local::snapshot& local ) const
	{
		timing_result out{ .should_wait = false };

		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.shot_timing.value || !best.valid || best.is_lethal( ) )
		{
			return out;
		}

		if ( this->m_hold_pawn )
		{
			const auto still_alive = std::any_of( candidates.begin( ), candidates.end( ), [ & ]( const candidate& c )
				{
					return c.pawn == this->m_hold_pawn;
				} );

			if ( !still_alive || this->m_hold_ticks >= config.shot_timing_max_hold.value )
			{
				return out;
			}

			out.should_wait = true;
			return out;
		}



		const auto lookahead = std::clamp( config.shot_timing_lookahead.value, 1, 32 );
		const auto eye = g_shared.get_shoot_position( );

		for ( auto& cand : candidates )
		{
			if ( !cand.pawn )
			{
				continue;
			}

			const auto backwards_aa = [ & ]( ) -> bool
			{
				if ( !cand.record_count || !cand.records[ 0 ] || !cand.records[ 0 ]->valid )
				{
					return false;
				}

				const auto angle_to_target = math::helpers::calculate_angle( eye, cand.records[ 0 ]->origin );
				return std::fabsf( math::helpers::normalize_yaw( cand.records[ 0 ]->rotation.y - angle_to_target.y ) ) > 100.0f;
			}( );

			auto extrap = g_shared.lc( ).extrapolate( cand.pawn, lookahead, true );
			if ( !extrap.has_value( ) )
			{
				continue;
			}

			this->m_timing_records.push_back( std::move( *extrap ) );
			auto& future = this->m_timing_records.back( );
			if ( !future.valid )
			{
				continue;
			}

			const auto game_scene_node = memory::read<std::uintptr_t>( cand.pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
			if ( !game_scene_node )
			{
				continue;
			}

			const auto hitbox_set = systems::g_hitboxes.query( game_scene_node );
			if ( hitbox_set.count <= 0 )
			{
				continue;
			}

			const auto skeleton = g_shared.lc( ).get_skeleton( future );
			const auto pen_ctx = g_shared.pen( ).prepare_target( cand.pawn, &future );

			const auto future_lethal = [ & ]( int hitbox_index ) -> bool
			{
				if ( backwards_aa && hitbox_index == 0 )
				{
					return false;
				}

				for ( const auto& entry : hitbox_set )
				{
					if ( entry.index != hitbox_index || entry.bone < 0 || entry.bone >= 28 )
					{
						continue;
					}

					const auto& bone = skeleton[ entry.bone ];
					if ( bone.position.length_sqr( ) < 1.0f )
					{
						continue;
					}

					const auto center = bone.rotation.rotate_vector( ( entry.mins + entry.maxs ) * 0.5f ) + bone.position;
					const auto aim = math::helpers::calculate_angle( eye, center );
					if ( math::helpers::angle_distance( ctx.view_angles, aim ) > config.max_fov )
					{
						continue;
					}

					shared::penetration::result pen{};
					if ( !g_shared.pen( ).run( eye, center, pen_ctx, local.pawn, local.team, pen ) )
					{
						continue;
					}

					if ( pen.damage >= static_cast< float >( cand.health ) )
					{
						return true;
					}
				}

				return false;
			};

			if ( future_lethal( 0 ) || future_lethal( 4 ) || future_lethal( 5 ) || future_lethal( 6 ) )
			{
				out.should_wait = true;
				break;
			}
		}

		return out;
	}

	bool rage::safe_line_clear( const math::vector3& eye, const target& tgt, const systems::local::snapshot& local ) const
	{
		if ( !tgt.valid || !tgt.hit.record )
		{
			return false;
		}

		const auto line = tgt.hit.position - eye;
		const auto line_len_sq = line.x * line.x + line.y * line.y + line.z * line.z;
		if ( line_len_sq < 1.0f )
		{
			return true;
		}

		const auto players = systems::g_entities.get_by_type( systems::entities::type::player );

		for ( const auto& p : players )
		{
			if ( !p.ptr || p.ptr == local.controller )
			{
				continue;
			}

			const auto pawn_handle = memory::read<std::uint32_t>( p.ptr + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
			const auto pawn = systems::g_entities.lookup( pawn_handle );
			if ( !pawn || pawn == local.pawn || pawn == tgt.hit.pawn )
			{
				continue;
			}

			const auto team = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
			if ( local.is_this_other_team( team ) )
			{
				continue;
			}

			const auto health = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
			if ( health <= 0 )
			{
				continue;
			}

			const auto pos = memory::read<math::vector3>( pawn + SCHEMA( "C_BaseEntity", "m_vecAbsOrigin"_hash ) );
			const auto rel = pos - eye;
			const auto t = std::clamp( ( rel.x * line.x + rel.y * line.y + rel.z * line.z ) / line_len_sq, 0.0f, 1.0f );
			const auto closest = eye + line * t;
			const auto dx = pos.x - closest.x;
			const auto dy = pos.y - closest.y;
			const auto dz = pos.z - closest.z;

			if ( dx * dx + dy * dy + dz * dz < 28.0f * 28.0f )
			{
				return false;
			}
		}

		return true;
	}

	void rage::run_gun( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local, bool allow_fire )
	{
		if ( !settings::g_combat.m_ragebot.enabled )
		{
			return;
		}

		auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		const auto autostop_enabled = config.autostop.value;

		this->update_miss_tracking( );




		const auto eff_min = config.min_damage_override.value
			? config.min_damage_override_value : config.min_damage.value;
		const auto delay_damage = eff_min >= 100;




		auto dt_hold_fire = false;
		if ( config.double_tap.value && this->m_double_tap_phase == 1 )
		{
			dt_hold_fire = true;
			this->m_double_tap_phase = 2;
			this->m_double_tap_tick = shared_ctx.current_tick;
		}




		const auto scan_range = shared_ctx.range > 0.0f ? shared_ctx.range + 120.0f : 0.0f;
		auto candidates = this->gather_candidates( local, scan_range > 0.0f ? scan_range * scan_range : 0.0f );

		{
			std::lock_guard lock( m_debug_mtx );
			m_debug_points.clear( );
		}

		if ( candidates.empty( ) )
		{
			return;
		}


		if ( this->m_last_target_pawn )
		{
			for ( auto i = 1u; i < candidates.size( ); ++i )
			{
				if ( candidates[ i ].pawn == this->m_last_target_pawn )
				{
					std::swap( candidates[ 0 ], candidates[ i ] );
					break;
				}
			}
		}




		{
			const auto& pre = systems::g_prediction.pre( );
			std::sort( candidates.begin( ) + ( this->m_last_target_pawn ? 1 : 0 ), candidates.end( ),
				[ & ]( const candidate& a, const candidate& b )
				{
					auto dist_sq = [ & ]( const candidate& c )
						{
							if ( c.record_count <= 0 || !c.records[ 0 ] )
							{
								return 1e30f;
							}
							const auto dx = c.records[ 0 ]->origin.x - pre.origin.x;
							const auto dy = c.records[ 0 ]->origin.y - pre.origin.y;
							const auto dz = c.records[ 0 ]->origin.z - pre.origin.z;
							return dx * dx + dy * dy + dz * dz;
						};
					return dist_sq( a ) < dist_sq( b );
				} );
		}




		constexpr int k_scan_budget = 180;
		constexpr int k_plan_budget = 40;
		std::atomic<int> trace_budget{ k_scan_budget };

		auto eye_candidates = g_shared.sh( ).get_candidates( );
		{



			const auto interpolated_eye = local.pawn ? g_shared.get_interpolated_shoot_position( local.pawn ) : math::vector3{};

			if ( eye_candidates.count > 0 )
			{
				eye_candidates.entries[ 0 ].position = interpolated_eye;
			}
			else if ( interpolated_eye.length_sqr( ) > 1.0f )
			{
				eye_candidates.entries[ 0 ].position = interpolated_eye;
				eye_candidates.entries[ 0 ].is_uninterpolated = true;
				eye_candidates.count = 1;
			}
		}

		if ( eye_candidates.count == 0 )
		{
			eye_candidates.entries[ 0 ].position = g_shared.get_shoot_position( );
			eye_candidates.entries[ 0 ].is_uninterpolated = true;
			eye_candidates.count = 1;
		}



		const auto keep_below_min = config.force_shot.value || config.double_tap.value;

		const auto scan_from_eye_candidates = [ & ]( const math::vector3& eye_offset, float inaccuracy, bool keep_below, int max_records = -1, std::atomic<int>* trace_budget_ptr = nullptr )
		{
			std::vector<scan_hit> hits_out;

			for ( auto i = 0; i < eye_candidates.count; ++i )
			{
				const auto eye = eye_candidates.entries[ i ].position + eye_offset;
				auto hits = this->scan_players( eye, inaccuracy, ctx, candidates, local, keep_below, max_records, trace_budget_ptr );
				auto found_direct{ false };

				for ( auto& hit : hits )
				{
					auto source_eye = eye_candidates.entries[ i ];
					source_eye.position = eye;
					hit.source_eye = source_eye;
					found_direct = found_direct || !hit.penetrated;
					hits_out.push_back( std::move( hit ) );
				}

				if ( found_direct )
				{
					break;
				}
			}

			return hits_out;
		};

		if ( config.no_spread.value )
		{
			shared_ctx.inaccuracy = g_shared.get_inaccuracy( false );
			auto all_hits = scan_from_eye_candidates( {}, shared_ctx.inaccuracy, keep_below_min, -1, &trace_budget );

			if ( all_hits.empty( ) )
			{
				return;
			}

			const auto best = this->select_best( ctx, all_hits, shared_ctx.inaccuracy );
			if ( !best.valid )
			{
				return;
			}




		if ( delay_damage && best.hit.damage < static_cast< float >( best.hit.health ) )
		{
			return;
		}




			{
				const auto ns_force = config.force_shot.value;
				if ( best.hit.below_min_damage && !ns_force )
				{
					return;
				}
			}

			if ( !allow_fire )
			{
				return;
			}



			if ( dt_hold_fire )
			{
				return;
			}

			logging::console::print( "[ragefire ns] est={:.0f} min={} below={} force={} ovr={} pen={}",
				best.hit.damage, config.min_damage.value, best.hit.below_min_damage ? 1 : 0,
				config.force_shot.value ? 1 : 0,
				config.min_damage_override.value ? 1 : 0, best.hit.penetrated ? 1 : 0 );
			this->fire_gun( cmd, best, false, best.hit.source_eye.position, local );

			if ( config.double_tap.value && best.hit.pawn )
			{
				this->m_double_tap_phase = 1;
				this->m_double_tap_pawn = best.hit.pawn;
			}
			return;
		}

		const auto primary_eye = eye_candidates.entries[ 0 ].position;
		const auto& prestate = systems::g_prediction.pre( );


		auto current_hits = scan_from_eye_candidates( {}, ctx.predicted_inaccuracy, keep_below_min, -1, &trace_budget );
		const auto best = this->select_best( ctx, current_hits, ctx.predicted_inaccuracy );




		{
			const auto now = shared_ctx.current_time;
			const auto scope_cooling = this->m_last_scope_press_time > 0.0f
				&& now >= this->m_last_scope_press_time
				&& ( now - this->m_last_scope_press_time ) < 0.2f;

			if ( settings::g_combat.m_autos.scope.value && settings::g_combat.m_ragebot.enabled
				&& shared_ctx.weapon_type == cstypes::weapon_type::sniper && !shared_ctx.is_scoped
				&& best.valid && !scope_cooling
				&& !( cmd->buttons.value & cstypes::command_buttons::in_second_attack ) )
			{
				cmd->buttons.value |= cstypes::command_buttons::in_second_attack;
				cmd->buttons.value_changed |= cstypes::command_buttons::in_second_attack;
				cmd->buttons.value_scroll |= cstypes::command_buttons::in_second_attack;
				this->m_last_scope_press_time = now;
			}
			else if ( shared_ctx.is_scoped || shared_ctx.weapon_type != cstypes::weapon_type::sniper )
			{
				this->m_last_scope_press_time = -1.0f;
			}
		}

		auto needed_hc = config.hitchance_override.value ? static_cast< float >( config.hitchance_override_value ) / 100.0f : static_cast< float >( config.hitchance ) / 100.0f;
		needed_hc = std::max( 0.0f, needed_hc - this->m_adaptive_hc_boost );
		const auto duckpeek_active = settings::g_combat.m_duckpeek.enabled.value && ctx.on_ground;
		const auto is_ducked = ( prestate.flags & cstypes::entity_flags::ducking ) != 0;

		const auto standing_inaccuracy = duckpeek_active ? this->get_standing_inaccuracy( local, ctx ) : ctx.predicted_inaccuracy;
		const auto standing_hc = best.valid
			? ( duckpeek_active ? this->evaluate_hitchance( best.hit, ctx, standing_inaccuracy ) : best.hitchance )
			: 0.0f;




		const auto need_for_hit = [ & ]( bool penetrated )
		{
			const auto need = needed_hc + ( penetrated ? 0.10f : 0.0f );
			return std::min( need, 0.99f );
		};

		const auto accurate = best.valid && standing_hc >= need_for_hit( best.hit.penetrated );




		auto force = best.valid && config.force_shot.value;

		const auto max_acc = g_shared.is_max_accuracy( standing_inaccuracy );



		const auto recoil_tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		const auto recoil_recovering = this->m_last_shot_tick != 0
			&& static_cast< std::uint32_t >( recoil_tick_base ) <= this->m_last_shot_tick + 3;
		if ( recoil_recovering )
		{
			force = false;
		}







		const auto backwards_force = [ & ]( ) -> bool
		{
			if ( !max_acc || recoil_recovering || !best.valid || best.hit.below_min_damage || !best.hit.record || !best.hit.record->valid )
			{
				return false;
			}

			const auto lethal = best.hit.damage >= static_cast< float >( best.hit.health );
			if ( !lethal && best.hitchance < needed_hc - 0.10f )
			{
				return false;
			}

			const auto eye_pos = best.hit.source_eye.position.length_sqr( ) > 0.1f
				? best.hit.source_eye.position
				: g_shared.get_shoot_position( );
			const auto angle_to_target = math::helpers::calculate_angle( eye_pos, best.hit.record->origin );
			return std::fabsf( math::helpers::normalize_yaw( best.hit.record->rotation.y - angle_to_target.y ) ) > 100.0f;
		}( );



		const auto hit_lethal = best.valid && best.hit.damage >= static_cast< float >( best.hit.health );
		const auto committed = !recoil_recovering && max_acc && best.valid && !best.hit.below_min_damage
			&& best.hit.record && best.hit.record->valid && !accurate
			&& ( hit_lethal || best.hitchance >= needed_hc - 0.10f );

		const auto shot_viable = accurate || force || backwards_force || committed;





		if ( this->m_stop_hold_ticks > 0 )
		{
			--this->m_stop_hold_ticks;
		}




		if ( autostop_enabled && !current_hits.empty( ) && ( !shot_viable || force ) && this->should_stop_movement( ctx ) )
		{
			const auto stop = this->predict_stop( ctx, primary_eye, local );
			auto stop_now{ false };

			if ( stop )
			{


				const auto future_offset = stop->eye - primary_eye;
				std::atomic<int> plan_budget{ k_plan_budget };
				auto planned_hits = scan_from_eye_candidates( future_offset, stop->inaccuracy, keep_below_min, 1, &plan_budget );
				const auto planned = this->select_best( ctx, planned_hits, stop->inaccuracy );
				stop_now = planned.valid;
			}
			else
			{
				stop_now = best.valid;
			}

			if ( stop_now )
			{



				this->m_should_stop = true;
				this->m_stop_hold_ticks = std::max( this->m_stop_hold_ticks, 3 );
			}
			else if ( this->m_stop_hold_ticks <= 0 )
			{
				this->m_should_stop = false;
			}
		}
		else if ( this->m_stop_hold_ticks > 0 )
		{



			this->m_should_stop = true;
		}
		else
		{
			this->m_should_stop = false;
		}

		if ( !best.valid )
		{
			this->m_double_tap_phase = 0;
			this->m_double_tap_pawn = 0;
			this->m_stop_hold_ticks = 0;
			return;
		}

		if ( duckpeek_active && allow_fire )
		{
			if ( shot_viable )
			{
				this->m_release_duck_for_shot = true;
			}
			else if ( !this->m_duckpeek_reduck )
			{
				this->m_release_duck_for_shot = false;
			}
		}

		auto ready_to_fire = shot_viable && !dt_hold_fire;






		const auto lethal_now = best.valid && best.hit.damage >= static_cast< float >( best.hit.health )
			* ( best.hit.penetrated ? 1.0f : kLethalMargin );






		if ( delay_damage && !force && !lethal_now )
		{
			const auto same = best.valid && best.hit.pawn && best.hit.pawn == this->m_delay_pawn;
			if ( !same )
			{
				this->m_delay_pawn = best.valid ? best.hit.pawn : 0;
				this->m_delay_ticks = 0;
			}

			if ( ++this->m_delay_ticks <= 48 || eff_min > 100 )
			{
				ready_to_fire = false;
			}
			else
			{
				this->m_delay_ticks = 0;
			}
		}
		else
		{
			this->m_delay_ticks = 0;
			this->m_delay_pawn = 0;
		}






		auto double_tap_followup{ false };



		if ( config.double_tap.value && this->m_double_tap_phase == 2 && best.valid &&
			best.hit.pawn && best.hit.pawn == this->m_double_tap_pawn &&
			shared_ctx.current_tick != this->m_double_tap_tick &&
			shared_ctx.current_tick - this->m_double_tap_tick <= 32 &&
			best.hitchance >= need_for_hit( best.hit.penetrated ) * 0.5f &&
			!best.hit.below_min_damage &&
			g_shared.can_shoot( cmd, local.controller ) )
		{
			double_tap_followup = true;
		}
		else if ( this->m_double_tap_phase == 2 &&
			( !best.valid || !best.hit.pawn || best.hit.pawn != this->m_double_tap_pawn ||
				shared_ctx.current_tick - this->m_double_tap_tick > 32 ) )
		{
			this->m_double_tap_phase = 0;
			this->m_double_tap_pawn = 0;
		}
		ready_to_fire = ready_to_fire || double_tap_followup;





		if ( best.valid && best.hit.below_min_damage && !force && !double_tap_followup )
		{
			ready_to_fire = false;
		}

		if ( duckpeek_active )
		{
			if ( is_ducked )
			{
				ready_to_fire = false;
			}
			else
			{
				ready_to_fire = ready_to_fire && this->m_release_duck_for_shot;
			}
		}




		if ( ready_to_fire && !force && best.valid
			&& shared_ctx.weapon_type == cstypes::weapon_type::sniper
			&& !shared_ctx.is_scoped && settings::g_combat.m_autos.scope.value )
		{
			ready_to_fire = false;
		}






		if ( ready_to_fire && force && !backwards_force && !committed && best.valid && best.hitchance < need_for_hit( best.hit.penetrated ) * 0.7f )
		{
			ready_to_fire = false;
		}







		if ( ready_to_fire && !accurate && best.valid && best.hit.record )
		{
			const auto any_hc = g_shared.calculate_any_hitchance( best.hit.source_eye.position, best.hit.aim_angle, *best.hit.record, ctx.predicted_inaccuracy, ctx.spread, 96 );
			if ( any_hc < 0.5f )
			{
				ready_to_fire = false;
			}
		}





		if ( ready_to_fire && best.valid )
		{
			const auto cone_factor = this->validate_shot_cone( best, ctx, standing_inaccuracy, local );

			if ( accurate )
			{
				if ( standing_hc * cone_factor < need_for_hit( best.hit.penetrated ) )
				{
					ready_to_fire = false;
				}
			}
			else if ( cone_factor <= 0.0f )
			{
				ready_to_fire = false;
			}
		}



		if ( ready_to_fire && config.safe_line_check.value && best.hit.penetrated && !this->safe_line_clear( best.hit.source_eye.position, best, local ) )
		{
			ready_to_fire = false;
		}



		if ( config.shot_timing.value )
		{
			this->m_timing_records.clear( );
			const auto timing = this->evaluate_shot_timing( ctx, candidates, best, local );
			if ( timing.should_wait && ready_to_fire )
			{
				if ( this->m_hold_pawn == best.hit.pawn && this->m_hold_ticks >= config.shot_timing_max_hold.value )
				{
					this->m_hold_pawn = 0;
					this->m_hold_ticks = 0;
				}
				else
				{
					this->m_hold_pawn = best.hit.pawn;
					++this->m_hold_ticks;
					this->m_should_stop = true;
					ready_to_fire = false;
				}
			}
			else
			{
				this->m_hold_pawn = 0;
				this->m_hold_ticks = 0;
			}
		}

		if ( ready_to_fire && allow_fire )
		{
			logging::console::print( "[ragefire] est={:.0f} min={} below={} force={} dt={} ovr={} hc={:.0f} pen={}",
				best.hit.damage, config.min_damage.value, best.hit.below_min_damage ? 1 : 0,
				force ? 1 : 0, double_tap_followup ? 1 : 0, config.min_damage_override.value ? 1 : 0,
				best.hitchance * 100.0f, best.hit.penetrated ? 1 : 0 );
			this->fire_gun( cmd, best, !accurate && ( force || backwards_force || committed ), best.hit.source_eye.position, local );




			if ( config.double_tap.value && best.valid && best.hit.pawn )
			{
				this->m_double_tap_phase = 1;
				this->m_double_tap_pawn = best.hit.pawn;
			}
			else
			{
				this->m_double_tap_phase = 0;
				this->m_double_tap_pawn = 0;
			}

			if ( duckpeek_active )
			{
				this->m_duckpeek_reduck = true;
				this->m_release_duck_for_shot = false;
			}
		}
		else
		{
		}
	}

	void rage::run_taser( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local )
	{
		if ( !settings::g_combat.m_zeusbot.enabled )
		{
			return;
		}

		auto candidates = this->gather_candidates( local );
		if ( candidates.empty( ) )
		{
			return;
		}

		auto eye_candidates = g_shared.sh( ).get_candidates( );
		{
			const auto interpolated_eye = local.pawn ? g_shared.get_interpolated_shoot_position( local.pawn ) : math::vector3{};

			if ( eye_candidates.count > 0 )
			{
				eye_candidates.entries[ 0 ].position = interpolated_eye;
			}
			else if ( interpolated_eye.length_sqr( ) > 1.0f )
			{
				eye_candidates.entries[ 0 ].position = interpolated_eye;
				eye_candidates.entries[ 0 ].is_uninterpolated = true;
				eye_candidates.count = 1;
			}
		}

		if ( eye_candidates.count == 0 )
		{
			eye_candidates.entries[ 0 ].position = g_shared.get_shoot_position( );
			eye_candidates.entries[ 0 ].is_uninterpolated = true;
			eye_candidates.count = 1;
		}

		std::vector<scan_hit> all_hits;

		for ( auto i = 0; i < eye_candidates.count; ++i )
		{
			auto hits = this->scan_taser( eye_candidates.entries[ i ].position, ctx, candidates, local );

			for ( auto& h : hits )
			{
				h.source_eye = eye_candidates.entries[ i ];
				all_hits.push_back( std::move( h ) );
			}
		}

		if ( all_hits.empty( ) )
		{
			return;
		}

		target best{};

		for ( const auto& h : all_hits )
		{
			if ( !best.valid || h.score > best.score )
			{
				best.hit = h;
				best.hitchance = 1.0f;
				best.score = h.score;
				best.valid = true;
			}
		}

		if ( best.valid )
		{
			this->m_zeus_fired = true;
			this->fire_melee( cmd, best, local );
		}
	}

	void rage::run_knife( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local )
	{
		if ( !settings::g_combat.m_knifebot.enabled )
		{
			return;
		}

		const auto info = this->get_knife_info( local );
		if ( !info.can_slash && !info.can_stab )
		{
			return;
		}

		constexpr auto max_knife_dist_sq = 150.0f * 150.0f;
		auto candidates = this->gather_candidates( local, max_knife_dist_sq );
		if ( candidates.empty( ) )
		{
			return;
		}

		auto eye_candidates = g_shared.sh( ).get_candidates( );
		{
			const auto interpolated_eye = local.pawn ? g_shared.get_interpolated_shoot_position( local.pawn ) : math::vector3{};

			if ( eye_candidates.count > 0 )
			{
				eye_candidates.entries[ 0 ].position = interpolated_eye;
			}
			else if ( interpolated_eye.length_sqr( ) > 1.0f )
			{
				eye_candidates.entries[ 0 ].position = interpolated_eye;
				eye_candidates.entries[ 0 ].is_uninterpolated = true;
				eye_candidates.count = 1;
			}
		}

		if ( eye_candidates.count == 0 )
		{
			eye_candidates.entries[ 0 ].position = g_shared.get_shoot_position( );
			eye_candidates.entries[ 0 ].is_uninterpolated = true;
			eye_candidates.count = 1;
		}

		std::vector<scan_hit> all_hits;

		for ( auto i = 0; i < eye_candidates.count; ++i )
		{
			auto hits = this->scan_knife( eye_candidates.entries[ i ].position, ctx, info, candidates, local );

			for ( auto& h : hits )
			{
				h.source_eye = eye_candidates.entries[ i ];
				all_hits.push_back( std::move( h ) );
			}
		}

		if ( all_hits.empty( ) )
		{
			return;
		}

		target best{};
		target best_backstab{};

		for ( const auto& h : all_hits )
		{
			auto& dest = h.is_backstab ? best_backstab : best;

			if ( !dest.valid || h.score > dest.score )
			{
				dest.hit = h;
				dest.hitchance = 1.0f;
				dest.score = h.score;
				dest.valid = true;
			}
		}

		auto& chosen = best_backstab.valid ? best_backstab : best;
		if ( !chosen.valid )
		{
			return;
		}

		this->m_knife_attack = static_cast< std::uint8_t >( chosen.hit.attack_type );
		this->fire_melee( cmd, chosen, local );
	}

	void rage::auto_revolver( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local )
	{
		if ( !settings::g_combat.m_ragebot.enabled )
		{
			this->m_revolver_cock_ticks = 0;
			return;
		}

		if ( !g_shared.can_shoot( cmd, local.controller ) )
		{
			this->m_revolver_cock_ticks = 0;
			return;
		}

		const auto& config = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type );





		(void)config;




		if ( !settings::g_combat.m_autos.revolver.value )
		{
			this->m_revolver_cock_ticks = 0;
			this->run_gun( cmd, ctx, local );
			return;
		}

		constexpr auto cock_ticks{ 13 };
		if ( this->m_revolver_cock_ticks >= cock_ticks )
		{


			cmd->buttons.value &= ~cstypes::command_buttons::in_attack;
			cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
			cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_attack;
			cmd->csgo_user_cmd.set_attack1_start_history_index( -1 );
			this->m_revolver_cock_ticks = 0;

			this->run_gun( cmd, ctx, local );
			return;
		}



		this->run_gun( cmd, ctx, local, false );

		cmd->buttons.value |= cstypes::command_buttons::in_attack;
		cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
		cmd->buttons.value_scroll |= cstypes::command_buttons::in_attack;

		const auto history_index = cmd->csgo_user_cmd.input_history_size( ) - 1;
		if ( history_index >= 0 )
		{
			cmd->csgo_user_cmd.set_attack1_start_history_index( history_index );
		}

		++this->m_revolver_cock_ticks;
	}

	std::vector<rage::scan_hit> rage::scan_players( const math::vector3& eye, float inaccuracy, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local, bool keep_below_min, int max_records, std::atomic<int>* trace_budget ) const
	{




		std::vector<std::vector<scan_hit>> per_candidate( candidates.size( ) );




		const auto backtrack_on = settings::g_combat.m_lagcomp.backtrack.value;
		const auto refine_latency = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type ).refine_shot.value == 1;

		threadpool::parallel_for( 0, static_cast< int >( candidates.size( ) ), [ & ]( int begin, int end )
			{
				for ( auto ci = begin; ci < end; ++ci )
				{
					if ( trace_budget && trace_budget->load( std::memory_order_relaxed ) <= 0 )
					{
						break;
					}

					auto& cand = candidates[ static_cast< std::size_t >( ci ) ];
					auto& candidate_hits = per_candidate[ static_cast< std::size_t >( ci ) ];
					candidate_hits.reserve( 24 );




					const auto first_tick = cand.record_count > 0 && cand.records[ 0 ] ? cand.records[ 0 ]->tick : 0;

					const auto cap_ri = max_records >= 0 ? std::min( cand.record_count, max_records ) : cand.record_count;
					const auto max_ri = !backtrack_on ? 1 : cap_ri;
					for ( auto ri = 0; ri < cand.record_count && ri < max_ri; ++ri )
					{
						if ( !cand.records[ ri ] || !cand.records[ ri ]->valid )
						{
							continue;
						}

						if ( refine_latency && ri > 0 && cand.records[ ri ]->tick != first_tick )
						{
							break;
						}

						auto hits = this->scan_player( eye, inaccuracy, ctx, cand, cand.records[ ri ], local, keep_below_min, trace_budget );



						const auto has_direct_hit = std::any_of( hits.begin( ), hits.end( ), [ ]( const scan_hit& hit )
							{
								return !hit.penetrated && !hit.below_min_damage;
							} );

						for ( auto& h : hits )
						{
							candidate_hits.push_back( std::move( h ) );
						}



						if ( has_direct_hit )
						{
							break;
						}

						if ( trace_budget && trace_budget->load( std::memory_order_relaxed ) <= 0 )
						{
							break;
						}
					}
				}
			}, 1 );


		std::vector<scan_hit> flat;
		auto total_hits{ std::size_t{} };
		for ( const auto& hits : per_candidate )
		{
			total_hits += hits.size( );
		}
		flat.reserve( total_hits );

		for ( auto& v : per_candidate )
		{
			for ( auto& h : v )
			{
				flat.push_back( std::move( h ) );
			}
		}

		return flat;
	}

	std::vector<rage::scan_hit> rage::scan_player( const math::vector3& eye, float inaccuracy, const aim_context& ctx, candidate& cand, shared::lagcomp::record* record, const systems::local::snapshot& local, bool keep_below_min, std::atomic<int>* trace_budget ) const
	{

		if (!cand.pawn || cand.record_count <= 0 || cand.health <= 0)
			return {};

		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );

		const auto game_scene_node = memory::read<std::uintptr_t>( cand.pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
		const auto hitbox_set = systems::g_hitboxes.query( game_scene_node );
		const auto skeleton = g_shared.lc( ).get_skeleton( *record );
		const auto pen_ctx = g_shared.pen( ).prepare_target( cand.pawn, record );


		const auto force_body = config.body_aim.value || ( cand.health <= 35 && cand.armor > 0 );





		const auto eye_to_target_dist = ( record->origin - eye ).length( );
		const auto far_target = eye_to_target_dist > 1100.0f;



		const auto close_target = eye_to_target_dist < 300.0f;




		auto head_only{ false };
		if ( !force_body && config.hitboxes.values[ 0 ] )
		{
			for ( const auto& entry : hitbox_set )
			{
				if ( entry.index != 0 )
				{
					continue;
				}

				if ( entry.bone < 0 || entry.bone >= 28 )
				{
					break;
				}

				const auto& head_bone = skeleton[ entry.bone ];
				if ( head_bone.position.length_sqr( ) < 1.0f )
				{
					break;
				}

				const auto head_center = head_bone.rotation.rotate_vector( ( entry.mins + entry.maxs ) * 0.5f ) + head_bone.position;
				const auto head_aim = math::helpers::calculate_angle( eye, head_center );

				if ( math::helpers::angle_distance( ctx.view_angles, head_aim ) > config.max_fov )
				{
					break;
				}

				if ( trace_budget && trace_budget->load( std::memory_order_relaxed ) <= 0 )
				{
					break;
				}

				shared::penetration::result head_pen{};
				if ( !g_shared.pen( ).run( eye, head_center, pen_ctx, local.pawn, local.team, head_pen ) )
				{
					if ( trace_budget )
					{
						trace_budget->fetch_sub( 1, std::memory_order_relaxed );
					}
					break;
				}

				if ( trace_budget )
				{
					trace_budget->fetch_sub( 1, std::memory_order_relaxed );
				}

				if ( head_pen.penetrated || head_pen.damage < static_cast< float >( cand.health ) )
				{
					break;
				}


				{
					const auto hp_now = static_cast< float >( cand.health );
					const auto floor = hp_now < cand.min_damage ? hp_now * kLethalMargin : cand.min_damage;
					if ( head_pen.damage < floor && head_pen.damage < hp_now )
					{
						break;
					}
				}

				const auto need_hc = config.hitchance_override.value
					? static_cast< float >( config.hitchance_override_value ) / 100.0f
					: static_cast< float >( config.hitchance ) / 100.0f;

				if ( g_shared.calculate_hitchance( eye, head_aim, entry, head_bone, inaccuracy, ctx.spread, 64 ) < need_hc )
				{
					break;
				}

				head_only = true;
				break;
			}
		}

		std::array<int, 19> scan_order{};
		auto scan_count{ 0 };

		if ( head_only )
		{
			scan_order[ scan_count++ ] = 0;
		}
		else
		{
		if ( !force_body && config.hitboxes.values[ 0 ] )
		{
			scan_order[ scan_count++ ] = 0;
		}

		if ( config.hitboxes.values[ 1 ] )
		{
			scan_order[ scan_count++ ] = 4;
			scan_order[ scan_count++ ] = 5;
			scan_order[ scan_count++ ] = 6;
		}

		if ( config.hitboxes.values[ 2 ] )
		{
			scan_order[ scan_count++ ] = 3;
			scan_order[ scan_count++ ] = 2;
		}

		if ( config.hitboxes.values[ 3 ] && !far_target )
		{
			for ( auto idx : { 13, 14, 15, 16, 17, 18 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}

		if ( config.hitboxes.values[ 4 ] && !far_target )
		{
			for ( auto idx : { 7, 8, 9, 10 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}

		if ( config.hitboxes.values[ 5 ] && !far_target )
		{
			for ( auto idx : { 11, 12 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}
		}



		if ( scan_count == 0 )
		{
			if ( !force_body )
			{
				scan_order[ scan_count++ ] = 0;
			}

			for ( auto idx : { 4, 5, 6, 3, 2 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}

		struct trace_point
		{
			math::vector3 position;
			int hitbox_index;
			int bone_index;
			systems::hitboxes::entry hitbox;
			bool is_center;
		};

		std::vector<trace_point> points;
		points.reserve( static_cast< std::size_t >( scan_count ) * 12 );

		for ( auto idx = 0; idx < scan_count; ++idx )
		{
			const auto hitbox_index = scan_order[ idx ];
			const systems::hitboxes::entry* hb{ nullptr };

			for ( const auto& entry : hitbox_set )
			{
				if ( entry.index == hitbox_index )
				{
					hb = &entry;
					break;
				}
			}

			if ( !hb || hb->bone < 0 || hb->bone >= 28 )
			{
				continue;
			}

			const auto& bone = skeleton[ hb->bone ];
			if ( bone.position.length_sqr( ) < 1.0f )
			{
				continue;
			}

			const auto hitbox_center = ( hb->mins + hb->maxs ) * 0.5f;
			const auto center = bone.rotation.rotate_vector( hitbox_center ) + bone.position;

			trace_point cp{};
			cp.position = center;
			cp.hitbox_index = hitbox_index;
			cp.bone_index = hb->bone;
			cp.hitbox = *hb;
			cp.is_center = true;
			points.push_back( cp );

			if ( config.debug_multipoints.value )
			{
				std::lock_guard lock( m_debug_mtx );
				m_debug_points.push_back( { center, hitbox_index, true } );
			}





			auto multipoint_slot = [ ]( int hitbox_index ) -> int
			{
				if ( hitbox_index == 0 ) return 0;
				if ( hitbox_index >= 4 && hitbox_index <= 6 ) return 1;
				if ( hitbox_index == 2 || hitbox_index == 3 ) return 2;
				if ( hitbox_index >= 13 && hitbox_index <= 18 ) return 3;
				if ( hitbox_index >= 7 && hitbox_index <= 10 ) return 4;
				if ( hitbox_index == 11 || hitbox_index == 12 ) return 5;
				return -1;
			};

			const auto mp_slot = multipoint_slot( hitbox_index );
			if ( !close_target && config.pointscale > 0.0f && ( mp_slot < 0 || config.multipoint.values[ mp_slot ] ) )
			{
				const auto mps = this->generate_multipoints( *hb, center, bone.rotation, config.pointscale, eye, inaccuracy );





				const auto first_of_hitbox = points.empty( ) ? points.size( ) : points.size( ) - 1;

				for ( const auto& mp : mps )
				{
					const auto duplicate = std::any_of( points.begin( ) + first_of_hitbox, points.end( ), [ & ]( const trace_point& point )
						{
							return point.hitbox_index == hitbox_index && ( point.position - mp ).length_sqr( ) < 0.01f;
						} );
					if ( duplicate )
					{
						continue;
					}

					trace_point tp{};
					tp.position = mp;
					tp.hitbox_index = hitbox_index;
					tp.bone_index = hb->bone;
					tp.hitbox = *hb;
					tp.is_center = false;
					points.push_back( tp );

				if ( config.debug_multipoints.value )
				{
					std::lock_guard lock( m_debug_mtx );
					m_debug_points.push_back( { mp, hitbox_index, false } );
				}
			}
		}
	}



		if ( points.size( ) > 48 )
		{
			points.resize( 48 );
		}

		if ( points.empty( ) )
		{
			return {};
		}

		std::vector<scan_hit> results;
		results.reserve( static_cast< std::size_t >( scan_count ) * 2 );
		std::array<bool, 19> center_sufficient{};

		for ( const auto& tp : points )
		{
			if ( !tp.is_center && tp.hitbox_index >= 0 && tp.hitbox_index < static_cast< int >( center_sufficient.size( ) ) && center_sufficient[ tp.hitbox_index ] )
			{
				continue;
			}

			const auto aim = math::helpers::calculate_angle( eye, tp.position );
			const auto fov = math::helpers::angle_distance( ctx.view_angles, aim );

			if ( fov > config.max_fov )
			{
				continue;
			}




			if ( trace_budget && trace_budget->load( std::memory_order_relaxed ) <= 0 )
			{
				break;
			}

			shared::penetration::result pen{};
			if ( !g_shared.pen( ).run( eye, tp.position, pen_ctx, local.pawn, local.team, pen ) )
			{
				if ( trace_budget )
				{
					trace_budget->fetch_sub( 1, std::memory_order_relaxed );
				}
				continue;
			}

			if ( trace_budget )
			{
				trace_budget->fetch_sub( 1, std::memory_order_relaxed );
			}








			if ( pen.damage < 1.0f )
			{
				continue;
			}

			const auto hp_now = static_cast< float >( cand.health );
			const auto wall_m = pen.penetrated ? 1.25f : 1.0f;
			const auto floor = hp_now < cand.min_damage ? hp_now * kLethalMargin
				: cand.min_damage * wall_m;
			const auto below_min_damage = pen.damage < floor
				&& pen.damage < hp_now;

			if ( below_min_damage && !keep_below_min )
			{
				continue;
			}


			if ( !config.autowall.value && pen.penetrated )
			{
				continue;
			}

			if ( !tp.is_center && tp.hitbox_index == 0 )
			{
				if ( pen.hitgroup != systems::g_hitboxes.hitgroup_from_hitbox( tp.hitbox_index ) )
				{
					continue;
				}
			}





		if ( tp.is_center && tp.hitbox_index >= 0 && tp.hitbox_index < static_cast< int >( center_sufficient.size( ) ) )
		{
			center_sufficient[ tp.hitbox_index ] = !pen.penetrated && pen.damage >= static_cast< float >( cand.health ) * kLethalMargin;
		}

			scan_hit h{};
			h.position = tp.position;
			h.aim_angle = aim;
			h.damage = pen.damage;
			h.fov = fov;
			h.hitbox_index = tp.hitbox_index;
			h.hitgroup = pen.hitgroup;
			h.bone_index = tp.bone_index;
			h.hitbox = tp.hitbox;
			h.is_center = tp.is_center;
			h.penetrated = pen.penetrated;
			h.below_min_damage = below_min_damage;
			h.pawn = cand.pawn;
			h.health = cand.health;
			h.record = record;

			results.push_back( h );
		}

		return results;
	}

	rage::target rage::select_best( const aim_context& aim_ctx, const std::vector<scan_hit>& hits, float eval_inaccuracy ) const
	{
		auto hitgroup_priority = [ ]( int hitbox_index ) -> int
			{
				if ( hitbox_index == 0 ) { return 4; }
				if ( hitbox_index >= 1 && hitbox_index <= 6 ) { return 3; }
				if ( hitbox_index >= 13 && hitbox_index <= 18 ) { return 2; }
				if ( hitbox_index >= 7 && hitbox_index <= 12 ) { return 1; }
				return 0;
			};

		struct record_group
		{
			shared::lagcomp::record* record;
			std::vector<int> hit_indices;
		};

		std::vector<record_group> groups;
		groups.reserve( 16 );

		for ( auto i = 0; i < static_cast< int >( hits.size( ) ); ++i )
		{
			auto rec = hits[ i ].record;
			auto found{ false };

			for ( auto& g : groups )
			{
				if ( g.record == rec )
				{
					g.hit_indices.push_back( i );
					found = true;
					break;
				}
			}

			if ( !found )
			{
				record_group g{};
				g.record = rec;
				g.hit_indices.reserve( 16 );
				g.hit_indices.push_back( i );
				groups.push_back( std::move( g ) );
			}
		}

		constexpr auto top_k_per_record{ 12 };

			auto cheap_score = [ & ]( const scan_hit& h ) -> float
			{
				const auto is_head = h.hitbox_index == 0;
				const auto lethal_bonus = h.damage >= static_cast< float >( h.health ) ? 100000.0f : 0.0f;
				const auto direct_bonus = h.penetrated ? 0.0f : 5000.0f;
				const auto center_bonus = h.is_center ? 2000.0f : 0.0f;
				const auto head_bonus = is_head ? 30000.0f : 0.0f;


				const auto min_dmg_bonus = !h.below_min_damage ? 8000.0f : 0.0f;

				return lethal_bonus + head_bonus + direct_bonus + center_bonus + min_dmg_bonus + h.damage * 20.0f +
					static_cast< float >( hitgroup_priority( h.hitbox_index ) ) * 200.0f - h.fov;
			};

		for ( auto& group : groups )
		{
			if ( static_cast< int >( group.hit_indices.size( ) ) <= top_k_per_record )
			{
				continue;
			}

			std::partial_sort
			(
				group.hit_indices.begin( ),
				group.hit_indices.begin( ) + top_k_per_record,
				group.hit_indices.end( ),
				[ & ]( int a, int b ) { return cheap_score( hits[ a ] ) > cheap_score( hits[ b ] ); }
			);

			group.hit_indices.resize( top_k_per_record );
		}

			struct evaluated_hit
			{
				int hit_index;
				float hitchance;
				float score;
				bool passes{};
				int group_idx{};
			};

			std::vector<evaluated_hit> evaluated;
			evaluated.reserve( hits.size( ) );

			const auto& config = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type );
			auto needed_hc = config.hitchance_override.value
				? static_cast< float >( config.hitchance_override_value ) / 100.0f
				: static_cast< float >( config.hitchance ) / 100.0f;


			needed_hc = std::max( 0.0f, needed_hc - this->m_adaptive_hc_boost );

			const auto need_for_hit = [ & ]( bool penetrated )
			{
				return std::min( needed_hc + ( penetrated ? 0.10f : 0.0f ), 0.99f );
			};







			const auto coarse_samples = config.no_spread.value ? 1 : 48;
			constexpr auto fine_samples = 256;

			constexpr auto hc_slack = 0.07f;






		auto newest_overall_tick{ 0 };
		for ( const auto& h : hits )
		{
			if ( h.record )
			{
				newest_overall_tick = std::max( newest_overall_tick, h.record->tick );
			}
		}





		std::unordered_map<std::uintptr_t, int> newest_by_pawn{};
		newest_by_pawn.reserve( hits.size( ) );
		for ( const auto& h : hits )
		{
			if ( h.record )
			{
				auto& slot = newest_by_pawn[ h.pawn ];
				slot = std::max( slot, h.record->tick );
			}
		}

		for ( auto gi = 0; gi < static_cast< int >( groups.size( ) ); ++gi )
		{
			auto& group = groups[ static_cast< std::size_t >( gi ) ];
			if ( !group.record || !group.record->valid )
			{
				continue;
			}

			for ( const auto idx : group.hit_indices )
			{
				const auto& h = hits[ idx ];

				if ( !h.record || !h.record->valid )
				{
					continue;
				}

				if ( config.refine_shot.value == 1 )
				{
					const auto newest_it = newest_by_pawn.find( h.pawn );
					if ( newest_it == newest_by_pawn.end( ) || h.record->tick != newest_it->second )
					{
						continue;
					}
				}

				if ( h.bone_index < 0 || h.bone_index >= 28 )
				{
					continue;
				}

				const auto& bone = group.record->bones[ h.bone_index ];
				const auto hc = config.no_spread.value
					? 1.0f
					: g_shared.calculate_hitchance( h.source_eye.position, h.aim_angle, h.hitbox, bone, eval_inaccuracy, aim_ctx.spread, coarse_samples );
			const auto hp = static_cast< float >( h.health );




			const auto is_extrapolated = h.record->extrapolated;



			const auto can_kill = !is_extrapolated && h.damage >= hp * ( h.penetrated ? 1.0f : kLethalMargin );
			const auto passes_hitchance = !is_extrapolated && ( config.no_spread.value || hc >= need_for_hit( h.penetrated ) );
			auto score = passes_hitchance ? 1000000.0f : 0.0f;





				if ( h.below_min_damage )
				{
					score -= 2000000.0f;
				}

				if ( can_kill )
				{
					score += 100000.0f + hc * 10000.0f;
				}
				else
				{
					score += h.damage * hc * 100.0f + h.damage * 5.0f;
				}

			score += h.penetrated ? 0.0f : 250.0f;
			score += h.is_center ? 50.0f : 0.0f;




			if ( config.prefer_head.value && !config.body_aim.value && h.hitbox_index == 0 )
			{
				score += 15000.0f;
			}
		score += static_cast< float >( hitgroup_priority( h.hitbox_index ) ) * 2.0f;
		score -= h.fov * 0.1f;




		score -= static_cast< float >( newest_overall_tick - h.record->tick ) * 8.0f;





		{
			const auto nt = newest_by_pawn.find( h.pawn );
			if ( nt != newest_by_pawn.end( ) )
			{
				const auto stale_ticks = static_cast< float >( nt->second - h.record->tick );
				const auto stale_cost = stale_ticks * ( can_kill ? 25.0f : 120.0f );
				score -= stale_cost;
			}
		}

		if ( is_extrapolated )
		{
			score -= 20000.0f;
		}



		{
			const auto is_head = h.hitbox_index == 0;
			if ( config.baim_if_head_low_hc.value && is_head && hc < config.baim_head_hc_threshold.value )
			{
				if ( !can_kill )
				{
					score -= 5000000.0f;
				}
			}



			if ( can_kill && !h.penetrated )
			{
				score += 20000.0f;
			}
		}




				switch ( config.prefer.value )
				{
				case 1:
					score += ( 100.0f - hp ) * 100.0f;
					break;
				case 2:
				{
					const auto dist = std::sqrtf( ( h.source_eye.position - h.position ).length_sqr( ) );
					score += std::max( 0.0f, 2000.0f - dist ) * 5.0f;
					break;
				}
				case 3:
					score -= h.fov * 50.0f;
					break;
				default:
					break;
				}

				evaluated.push_back( evaluated_hit{ idx, hc, score, passes_hitchance, gi } );
			}
		}






		if ( !config.no_spread.value && !evaluated.empty( ) )
		{
			for ( auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				if ( !h.record || !h.record->valid || h.bone_index < 0 || h.bone_index >= 28 )
				{
					continue;
				}

				const auto bar = need_for_hit( h.penetrated );
				if ( std::fabs( e.hitchance - bar ) > hc_slack )
				{
					continue;
				}

				const auto hc_old = e.hitchance;
				const auto hc_new = g_shared.calculate_hitchance( h.source_eye.position, h.aim_angle, h.hitbox, h.record->bones[ h.bone_index ], eval_inaccuracy, aim_ctx.spread, fine_samples );

				const auto hp = static_cast< float >( h.health );
				const auto is_extrapolated = h.record->extrapolated;
				const auto passes_old = e.passes;
				const auto passes_new = !is_extrapolated && hc_new >= bar;
				const auto can_kill = !is_extrapolated && h.damage >= hp * kLethalMargin;



				if ( passes_new != passes_old )
				{
					e.score += passes_new ? 1000000.0f : -1000000.0f;
				}
				e.score += can_kill
					? ( hc_new - hc_old ) * 10000.0f
					: h.damage * ( hc_new - hc_old ) * 100.0f;

			e.hitchance = hc_new;
			e.passes = passes_new;
		}
	}





		if ( !groups.empty( ) )
		{
			std::vector<bool> head_reliable( groups.size( ), false );
			for ( const auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				if ( h.hitbox_index == 0 && e.hitchance >= needed_hc && h.damage >= static_cast< float >( h.health ) )
				{
					head_reliable[ static_cast< std::size_t >( e.group_idx ) ] = true;
				}
			}

			std::vector<float> best_body_damage( groups.size( ), 0.0f );
			for ( const auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				if ( h.hitbox_index != 0 && !h.below_min_damage )
				{
					auto& slot = best_body_damage[ static_cast< std::size_t >( e.group_idx ) ];
					slot = std::max( slot, h.damage );
				}
			}

			for ( auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				const auto is_head = h.hitbox_index == 0;

				if ( head_reliable[ static_cast< std::size_t >( e.group_idx ) ] )
				{
					continue;
				}

				if ( is_head )
				{
					if ( e.hitchance < needed_hc )
					{
						e.score -= 5000000.0f;
					}
				}
				else if ( !h.below_min_damage && h.damage >= best_body_damage[ static_cast< std::size_t >( e.group_idx ) ] )
				{
					e.score += 6000000.0f + h.damage * 20.0f;
				}
			}
		}

		target best{};

		for ( const auto& e : evaluated )
		{
			if ( e.hit_index < 0 || e.hit_index >= static_cast< int >( hits.size( ) ) )
			{
				continue;
			}

			const auto& h = hits[ e.hit_index ];

			if ( !h.record || !h.record->valid )
			{
				continue;
			}

			auto is_better = !best.valid || e.score > best.score;
			if ( best.valid && std::fabsf( e.score - best.score ) < 0.01f )
			{
				if ( h.record->tick != best.hit.record->tick )
				{
					is_better = h.record->tick > best.hit.record->tick;
				}
				else if ( h.is_center != best.hit.is_center )
				{
					is_better = h.is_center;
				}
				else
				{
					is_better = h.fov < best.hit.fov;
				}
			}

			if ( is_better )
			{
				best.hit = h;
				best.hitchance = e.hitchance;
				best.score = e.score;
				best.valid = true;
			}
		}

		return best;
	}

	float rage::evaluate_hitchance( const scan_hit& hit, const aim_context& ctx, float inaccuracy ) const
	{
		if ( !hit.record || !hit.record->valid || hit.bone_index < 0 || hit.bone_index >= 28 )
		{
			return 0.0f;
		}

		return g_shared.calculate_hitchance( hit.source_eye.position, hit.aim_angle, hit.hitbox, hit.record->bones[ hit.bone_index ], inaccuracy, ctx.spread );
	}

	float rage::validate_shot_cone( const target& tgt, const aim_context& ctx, float inaccuracy, const systems::local::snapshot& local ) const
	{
		if ( !tgt.valid || !tgt.hit.record )
		{
			return 1.0f;
		}






		constexpr auto ring_samples = 8;
		constexpr auto sample_count = ring_samples + 1;

		const auto pen_ctx = g_shared.pen( ).prepare_target( tgt.hit.pawn, tgt.hit.record );

		const auto distance = ( tgt.hit.position - tgt.hit.source_eye.position ).length( );
		const auto spread_radius = std::tanf( inaccuracy + ctx.spread ) * distance;

		math::vector3 right{}, up{};
		math::helpers::angle_vectors_left( tgt.hit.aim_angle, nullptr, &right, &up );

		auto passing = 0;

		for ( auto i = 0; i < sample_count; ++i )
		{
			auto offset_x = 0.0f;
			auto offset_y = 0.0f;

			if ( i < ring_samples )
			{
				const auto angle = static_cast< float >( i ) / static_cast< float >( ring_samples ) * 6.2831853f;
				offset_x = std::cosf( angle ) * spread_radius * 0.5f;
				offset_y = std::sinf( angle ) * spread_radius * 0.5f;
			}

			const auto sample_target = tgt.hit.position + right * offset_x + up * offset_y;

			shared::penetration::result pen{};
			if ( g_shared.pen( ).run( tgt.hit.source_eye.position, sample_target, pen_ctx, local.pawn, local.team, pen )
				&& pen.damage >= 1.0f )
			{
				++passing;
			}
		}

		return static_cast< float >( passing ) / static_cast< float >( sample_count );
	}

	float rage::get_standing_inaccuracy( const systems::local::snapshot& local, const aim_context& ctx ) const
	{
		const auto& prestate = systems::g_prediction.pre( );
		auto velocity = prestate.networked_velocity;
		velocity.z = 0.0f;

		const auto speed = velocity.length_2d( );
		if ( speed > ctx.accurate_threshold )
		{
			return g_shared.get_inaccuracy_at_velocity( local.pawn, velocity );
		}

		const auto& shared_ctx = g_shared.ctx( );
		if ( !shared_ctx.weapon_vdata )
		{
			return ctx.predicted_inaccuracy;
		}

		const auto inaccuracy_stand = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyStand"_hash ) );
		return std::max( inaccuracy_stand, g_shared.get_inaccuracy_at_velocity( local.pawn, velocity ) );
	}

	std::vector<rage::scan_hit> rage::scan_taser( const math::vector3& eye, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		std::vector<scan_hit> results;

		for ( auto& cand : candidates )
		{
			for ( auto ri = 0; ri < cand.record_count; ++ri )
			{
				auto* record = cand.records[ ri ];
				if ( !record || !record->valid )
				{
					continue;
				}

				const auto game_scene_node = memory::read<std::uintptr_t>( cand.pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
				if ( !game_scene_node )
				{
					continue;
				}

				const auto hitbox_set = systems::g_hitboxes.query( game_scene_node );
				if ( hitbox_set.count <= 0 )
				{
					continue;
				}

				record->apply( );
				const auto skeleton = g_shared.lc( ).get_skeleton( *record );

				for ( auto i = 0; i < hitbox_set.count; ++i )
				{
					const auto& hb = hitbox_set.entries[ i ];

					if ( hb.bone < 0 || hb.bone >= 28 )
					{
						continue;
					}

					const auto& bone = skeleton[ hb.bone ];
					if ( bone.position.length_sqr( ) < 1.0f )
					{
						continue;
					}

					const auto center = bone.rotation.rotate_vector( ( hb.mins + hb.maxs ) * 0.5f ) + bone.position;
					const auto aim = math::helpers::calculate_angle( eye, center );
					const auto fov = math::helpers::angle_distance( ctx.view_angles, aim );

					if ( fov > settings::g_combat.m_zeusbot.max_fov )
					{
						continue;
					}

					math::vector3 forward{};
					math::helpers::angle_vectors_left( aim, &forward );

					const auto trace = this->trace_taser_hit( eye, forward, shared_ctx.range * 0.85f, cand.pawn, local.pawn );
					if ( trace.hit_entity != cand.pawn )
					{
						continue;
					}

					const auto dist = ( center - eye ).length( );
					const auto range_fraction = dist / shared_ctx.range;

					scan_hit h{};
					h.position = center;
					h.aim_angle = aim;
					h.damage = 500.0f;
					h.score = ( 10000.0f - dist ) * ( range_fraction > 0.92f ? 0.8f : 1.0f );
					h.fov = fov;
					h.hitbox_index = hb.index;
					h.hitgroup = systems::g_hitboxes.hitgroup_from_hitbox( hb.index );
					h.bone_index = hb.bone;
					h.hitbox = hb;
					h.is_center = true;
					h.pawn = cand.pawn;
					h.health = cand.health;
					h.record = record;

					results.push_back( h );
				}

				record->restore( );
			}
		}

		return results;
	}

	rage::knife_info rage::get_knife_info( const systems::local::snapshot& local ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		const auto next_primary = memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash ) );
		const auto next_secondary = memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextSecondaryAttackTick"_hash ) );
		const auto last_shot_time = memory::read<float>( shared_ctx.weapon + SCHEMA( "C_CSWeaponBase", "m_fLastShotTime"_hash ) );
		const auto cur_time = static_cast< float >( tick_base ) * cstypes::tick_interval;

		return knife_info
		{
			.can_slash = tick_base >= next_primary,
			.can_stab = tick_base >= next_secondary,
			.charged = ( cur_time - last_shot_time ) > 0.4f,
			.armor_ratio = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flArmorRatio"_hash ) )
		};
	}

	std::vector<rage::scan_hit> rage::scan_knife( const math::vector3& eye, const aim_context& ctx, const knife_info& info, std::vector<candidate>& candidates, const systems::local::snapshot& local ) const
	{
		constexpr auto stab_range{ 50.0f };
		constexpr auto slash_range{ 66.0f };

		std::vector<scan_hit> results;

		for ( auto& cand : candidates )
		{
			const auto eye_angles = memory::read<math::vector3>( cand.pawn + SCHEMA( "C_CSPlayerPawn", "m_angEyeAngles"_hash ) );
			const auto hp = static_cast< float >( cand.health );

			const auto frontal_slash_dmg = this->get_knife_damage( info.charged ? 40.0f : 25.0f, cand.armor, info.armor_ratio );
			const auto frontal_stab_dmg = this->get_knife_damage( 65.0f, cand.armor, info.armor_ratio );
			const auto frontal_can_kill = ( info.can_slash && frontal_slash_dmg >= hp ) || ( info.can_stab && frontal_stab_dmg >= hp );

			for ( auto ri = 0; ri < cand.record_count; ++ri )
			{
				auto* record = cand.records[ ri ];
				if ( !record || !record->valid )
				{
					continue;
				}

				const auto game_scene_node = memory::read<std::uintptr_t>( cand.pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
				if ( !game_scene_node )
				{
					continue;
				}

				const auto hitbox_set = systems::g_hitboxes.query( game_scene_node );
				if ( hitbox_set.count <= 0 )
				{
					continue;
				}

				record->apply( );
				const auto skeleton = g_shared.lc( ).get_skeleton( *record );

				auto backstab{ false };
				{
					const auto delta = record->origin - systems::g_prediction.pre( ).origin;
					const auto dist_2d = std::sqrtf( delta.x * delta.x + delta.y * delta.y );

					if ( dist_2d > 0.001f )
					{
						const auto dir_x = delta.x / dist_2d;
						const auto dir_y = delta.y / dist_2d;

						math::vector3 body_forward{};
						math::helpers::angle_vectors_left( record->rotation, &body_forward );

						math::vector3 eye_forward{};
						math::helpers::angle_vectors_left( eye_angles, &eye_forward );

						backstab = ( dir_x * body_forward.x + dir_y * body_forward.y ) > 0.475f ||
							( dir_x * eye_forward.x + dir_y * eye_forward.y ) > 0.475f;
					}
				}

				const auto wait_for_backstab = backstab && !frontal_can_kill;

				for ( auto i = 0; i < hitbox_set.count; ++i )
				{
					const auto& hb = hitbox_set.entries[ i ];

					if ( hb.bone < 0 || hb.bone >= 28 )
					{
						continue;
					}

					const auto& bone = skeleton[ hb.bone ];
					if ( bone.position.length_sqr( ) < 1.0f )
					{
						continue;
					}

					const auto center = bone.rotation.rotate_vector( ( hb.mins + hb.maxs ) * 0.5f ) + bone.position;
					const auto dist = ( center - eye ).length( );
					const auto max_reach = info.can_slash ? slash_range : stab_range;

					if ( dist > max_reach )
					{
						continue;
					}

					const auto aim = math::helpers::calculate_angle( eye, center );
					const auto fov = math::helpers::angle_distance( ctx.view_angles, aim );

					if ( fov > settings::g_combat.m_knifebot.max_fov )
					{
						continue;
					}

					math::vector3 forward{};
					math::helpers::angle_vectors_left( aim, &forward );

					for ( const auto try_stab : { true, false } )
					{
						if ( try_stab && !info.can_stab )
						{
							continue;
						}

						if ( !try_stab && !info.can_slash )
						{
							continue;
						}

						const auto reach = try_stab ? stab_range : slash_range;
						if ( dist > reach )
						{
							continue;
						}

						const auto raw_dmg = try_stab ? ( backstab ? 180.0f : 65.0f ) : ( backstab ? 90.0f : ( info.charged ? 40.0f : 25.0f ) );
						const auto damage = this->get_knife_damage( raw_dmg, cand.armor, info.armor_ratio );
						const auto can_kill = damage >= hp;

						if ( wait_for_backstab && !can_kill )
						{
							continue;
						}

						const auto trace = this->trace_knife_hit( eye, forward, reach, cand.pawn, local.pawn );
						if ( trace.hit_entity != cand.pawn )
						{
							continue;
						}

						const auto reach_margin = 1.0f - ( dist / reach );

						scan_hit h{};
						h.position = center;
						h.aim_angle = aim;
						h.damage = damage;
						h.score = can_kill ? ( 10000.0f + damage * reach_margin ) : ( damage * 100.0f * reach_margin );
						h.fov = fov;
						h.hitbox_index = hb.index;
						h.hitgroup = systems::g_hitboxes.hitgroup_from_hitbox( hb.index );
						h.bone_index = hb.bone;
						h.hitbox = hb;
						h.is_center = true;
						h.is_backstab = backstab;
						h.attack_type = try_stab ? 1 : 0;
						h.pawn = cand.pawn;
						h.health = cand.health;
						h.record = record;

						results.push_back( h );
						break;
					}
				}

				record->restore( );
			}
		}

		return results;
	}

	void rage::fire_gun( systems::input::usercmd* cmd, const target& tgt, bool was_forced, const math::vector3& shoot_eye, const systems::local::snapshot& local )
	{
		if ( !tgt.hit.record || !tgt.hit.record->valid )
		{
			return;
		}

		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );




		const auto burst_ms = static_cast< float >( config.burst_delay.value );
		if ( burst_ms > 0.0f && this->m_last_shoot_time > 0.0f
			&& shared_ctx.current_time >= this->m_last_shoot_time
			&& ( shared_ctx.current_time - this->m_last_shoot_time ) * 1000.0f < burst_ms )
		{
			return;
		}

		this->m_firing_this_tick = true;

		const auto base = cmd->csgo_user_cmd.mutable_base( );
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		const auto aim_punch = g_shared.get_aim_punch( local.pawn );
		auto aim_angle = config.no_spread.value ? math::helpers::calculate_angle( shoot_eye, tgt.hit.position ) : tgt.hit.aim_angle;




		if ( !std::isfinite( aim_angle.x ) || !std::isfinite( aim_angle.y ) || !std::isfinite( aim_punch.x ) || !std::isfinite( aim_punch.y )
			|| !std::isfinite( shoot_eye.x ) || !std::isfinite( shoot_eye.y ) || !std::isfinite( shoot_eye.z ) )
		{
			this->m_firing_this_tick = false;
			return;
		}

		if ( !config.no_spread.value )
		{




			const auto fired_inaccuracy = g_shared.get_inaccuracy( false );
			const auto fired_spread = g_shared.get_spread( );
			const auto needed_hc = config.hitchance_override.value ? static_cast< float >( config.hitchance_override_value ) / 100.0f : static_cast< float >( config.hitchance ) / 100.0f;

			if ( tgt.hit.bone_index >= 0 && tgt.hit.bone_index < 28 )
			{
				const auto fired_hc = g_shared.calculate_hitchance( shoot_eye, aim_angle, tgt.hit.hitbox, tgt.hit.record->bones[ tgt.hit.bone_index ], fired_inaccuracy, fired_spread );
				if ( fired_hc < needed_hc )
				{
					this->m_firing_this_tick = false;
					return;
				}
			}
		}

		if ( config.no_spread.value )
		{
			auto stamp_tick = tick_base;
			auto stamp_frac{ 0.0f };

			if ( !tgt.hit.source_eye.is_uninterpolated )
			{
				auto tick_add = [ ]( int t, float f, int tick_delta, float frac_delta )
					{
						f += frac_delta;
						auto carry = static_cast< int >( std::floor( f ) );
						f -= static_cast< float >( carry );
						return std::pair{ t + tick_delta + carry, f };
					};

				std::tie( stamp_tick, stamp_frac ) = tick_add( tgt.hit.source_eye.player_tick, tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int, tgt.hit.source_eye.lerp_ticks_frac );
			}




			const auto seed_base = math::vector3{ aim_angle.x - aim_punch.x, aim_angle.y - aim_punch.y, 0.0f };
			const auto corrected = g_shared.find_spread_correction( seed_base, stamp_tick );
			if ( corrected.x == 0.0f && corrected.y == 0.0f && corrected.z == 0.0f )
			{



				this->m_firing_this_tick = false;
				return;
			}

			aim_angle = math::vector3{ corrected.x + aim_punch.x, corrected.y + aim_punch.y, corrected.z };
		}

		g_shared.last_shoot_tick( ) = tick_base;
		this->m_last_shoot_time = g_shared.ctx( ).current_time;
		this->m_last_shot_tick = static_cast< std::uint32_t >( tick_base );
		this->m_last_target_pawn = tgt.hit.pawn;
		this->m_last_target_hitbox = tgt.hit.hitbox_index;
		this->m_last_shot_pawn = tgt.hit.pawn;
		this->m_last_shot_health = tgt.hit.health;
		this->m_last_shot_tick_when = tick_base;

		features::misc::g_impacts.on_boom( tgt.hit.pawn, tgt.hit.hitgroup, tgt.hit.damage, tgt.hitchance, shared_ctx.inaccuracy, shared_ctx.spread, aim_angle, shoot_eye, tgt.hit.record->tick, g_shared.lc( ).get_skeleton( *tgt.hit.record ), was_forced );
		features::esp::player::g_chams.os ().push (tgt.hit.pawn);
		const auto record_time = cstypes::tick_fraction::from_value( tgt.hit.record->simulation_time / cstypes::tick_interval );
		const auto history_size = cmd->csgo_user_cmd.input_history_size( );
		for ( auto i = 0; i < history_size; ++i )
		{
			const auto entry = cmd->csgo_user_cmd.mutable_input_history( i );
			if ( !entry )
			{
				continue;
			}

			if ( const auto angles = entry->mutable_view_angles( ) )
			{
				angles->set_x( aim_angle.x - aim_punch.x );
				angles->set_y( aim_angle.y - aim_punch.y );

				if ( config.no_spread.value )
				{
					angles->set_z( aim_angle.z );
				}
			}

			entry->set_render_tick_count( record_time.tick + 1 );
			entry->set_render_tick_fraction( 0.0f );

			if ( !tgt.hit.source_eye.is_uninterpolated )
			{
				auto tick_add = [ ]( int t, float f, int tick_delta, float frac_delta )
					{
						f += frac_delta;
						auto carry = static_cast< int >( std::floor( f ) );
						f -= static_cast< float >( carry );
						return std::pair{ t + tick_delta + carry, f };
					};

				const auto [stamp_tick, stamp_frac] = tick_add( tgt.hit.source_eye.player_tick, tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int, tgt.hit.source_eye.lerp_ticks_frac );

				entry->set_player_tick_count( stamp_tick );
				entry->set_player_tick_fraction( stamp_frac );
			}

			if ( entry->has_sv_interp0( ) )
			{
				const auto interp = entry->mutable_sv_interp0( );
				interp->set_src_tick( -1 );
				interp->set_dst_tick( -1 );
				interp->set_frac( 0.0f );
			}

			if ( entry->has_sv_interp1( ) )
			{
				const auto interp = entry->mutable_sv_interp1( );
				interp->set_src_tick( -1 );
				interp->set_dst_tick( -1 );
				interp->set_frac( 0.0f );
			}

			if ( entry->has_cl_interp( ) )
			{
				const auto interp = entry->mutable_cl_interp( );
				interp->set_frac( 0.0f );
			}
		}











		const auto quick_revolver = g_shared.quick_revolver_active( );
		const auto fire_button = quick_revolver
			? cstypes::command_buttons::in_second_attack
			: cstypes::command_buttons::in_attack;

		cmd->buttons.value |= fire_button;
		cmd->buttons.value_changed |= fire_button;
		cmd->buttons.value_scroll |= fire_button;

		if ( history_size > 0 )
		{



			if ( quick_revolver )
			{
				cmd->csgo_user_cmd.set_attack2_start_history_index( history_size - 1 );



				cmd->csgo_user_cmd.set_attack1_start_history_index( -1 );
			}
			else
			{
				cmd->csgo_user_cmd.set_attack1_start_history_index( history_size - 1 );
			}
		}

		math::vector3 forward{};
		{
			if ( const auto angles = base->viewangles( ) )
			{
				math::helpers::angle_vectors_left( { angles->x( ), angles->y( ), angles->z( ) }, &forward );
			}
		}

		const auto punched_aim = math::vector3{ aim_angle.x - aim_punch.x, aim_angle.y - aim_punch.y, 0.0f };
		const auto facing_away = forward.dot( ( tgt.hit.record->origin - systems::g_prediction.pre( ).networked_origin ).normalized( ) ) < 0.707107f;

		auto command_aim = punched_aim;



		if ( facing_away && settings::g_combat.m_antiaim.hide_shots.value && !config.no_spread.value )
		{
			command_aim.x = 179.9f;
			command_aim.y = std::remainderf( punched_aim.y + 180.0f, 360.0f );
		}

		if ( const auto angles = base->mutable_viewangles( ) )
		{
			angles->set_x( command_aim.x );
			angles->set_y( command_aim.y );
		}

		if ( !config.silent.value )
		{
			systems::g_input.set_view_angles( punched_aim );
		}
	}

	void rage::fire_melee( systems::input::usercmd* cmd, const target& tgt, const systems::local::snapshot& local )
	{
		if ( !tgt.hit.record || !tgt.hit.record->valid )
		{
			return;
		}

		this->m_firing_this_tick = true;

		const auto base = cmd->csgo_user_cmd.mutable_base( );
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );

		g_shared.last_shoot_tick( ) = tick_base;

		const auto record_time = cstypes::tick_fraction::from_value( tgt.hit.record->simulation_time / cstypes::tick_interval );
		const auto history_index = cmd->csgo_user_cmd.input_history_size( ) - 1;
		const auto entry = history_index >= 0 ? cmd->csgo_user_cmd.mutable_input_history( history_index ) : nullptr;

		if ( entry )
		{
			if ( const auto angles = entry->mutable_view_angles( ) )
			{
				angles->set_x( tgt.hit.aim_angle.x );
				angles->set_y( tgt.hit.aim_angle.y );
			}

			entry->set_render_tick_count( record_time.tick + 1 );
			entry->set_render_tick_fraction( 0.0f );

			if ( !tgt.hit.source_eye.is_uninterpolated )
			{
				auto tick_add = [ ]( int t, float f, int tick_delta, float frac_delta )
					{
						f += frac_delta;
						auto carry = static_cast< int >( std::floor( f ) );
						f -= static_cast< float >( carry );
						return std::pair{ t + tick_delta + carry, f };
					};

				const auto [stamp_tick, stamp_frac] = tick_add( tgt.hit.source_eye.player_tick, tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int, tgt.hit.source_eye.lerp_ticks_frac );

				entry->set_player_tick_count( stamp_tick );
				entry->set_player_tick_fraction( stamp_frac );
			}

			if ( entry->has_sv_interp0( ) )
			{
				const auto interp = entry->mutable_sv_interp0( );
				interp->set_src_tick( -1 );
				interp->set_dst_tick( -1 );
				interp->set_frac( 0.0f );
			}

			if ( entry->has_sv_interp1( ) )
			{
				const auto interp = entry->mutable_sv_interp1( );
				interp->set_src_tick( -1 );
				interp->set_dst_tick( -1 );
				interp->set_frac( 0.0f );
			}

			if ( entry->has_cl_interp( ) )
			{
				const auto interp = entry->mutable_cl_interp( );
				interp->set_frac( 0.0f );
			}

		}

		const auto is_secondary = tgt.hit.attack_type == 1;
		const auto attack_button = is_secondary
			? cstypes::command_buttons::in_second_attack
			: cstypes::command_buttons::in_attack;

		cmd->buttons.value |= attack_button;
		cmd->buttons.value_changed |= attack_button;
		cmd->buttons.value_scroll |= attack_button;

		if ( history_index >= 0 )
		{
			if ( is_secondary )
			{
				cmd->csgo_user_cmd.set_attack2_start_history_index( history_index );
			}
			else
			{
				cmd->csgo_user_cmd.set_attack1_start_history_index( history_index );
			}
		}

		if ( const auto angles = base->mutable_viewangles( ) )
		{
			angles->set_x( tgt.hit.aim_angle.x );
			angles->set_y( tgt.hit.aim_angle.y );
		}
	}

	std::vector<math::vector3> rage::generate_multipoints( const systems::hitboxes::entry& hitbox, const math::vector3& center, const math::quaternion& bone_rot, float pointscale, const math::vector3& shoot_pos, float inaccuracy ) const
	{
		std::vector<math::vector3> out;

		auto scale = std::clamp( pointscale / 100.0f, 0.0f, 1.0f );
		if ( scale <= 0.01f )
		{
			return out;
		}

		const auto hb_mid   = ( hitbox.mins + hitbox.maxs ) * 0.5f;
		const auto capsule_a = center + bone_rot.rotate_vector( hitbox.mins - hb_mid );
		const auto capsule_b = center + bone_rot.rotate_vector( hitbox.maxs - hb_mid );


		const auto& config = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type );
		if ( config.dynamic_pointscale.value && hitbox.radius > 0.001f )
		{
			const auto cone = std::max( inaccuracy + g_shared.ctx( ).spread, 0.0f );
			const auto cone_radius = std::tanf( cone ) * ( center - shoot_pos ).length( );
			const auto automatic_scale = std::clamp( 0.9f - cone_radius / hitbox.radius, 0.0f, 1.0f );
			scale = std::min( scale, automatic_scale );

			if ( scale <= 0.01f )
			{
				return out;
			}
		}


		const auto shoot_dir = ( center - shoot_pos ).normalized( );
		const auto ang       = math::helpers::vector_to_angle( shoot_dir );

		math::vector3 left{}, up{};
		math::helpers::angle_vectors_left( ang, nullptr, &left, &up );


		const auto right = math::vector3{ -left.x, -left.y, -left.z };



		const auto surface_point = [ & ]( const math::vector3& direction ) -> math::vector3
		{
			const auto dir = direction.normalized( );

			if ( hitbox.radius > 0.001f )
			{
				const auto reach = ( capsule_b - capsule_a ).length( ) + hitbox.radius * 2.0f + 1.0f;
				const auto origin = center + dir * reach;
				const auto delta = dir * ( reach * -2.0f );
				auto fraction{ 1.0f };

				if ( g_shared.ray_vs_capsule( origin, delta, capsule_a, capsule_b, hitbox.radius, fraction ) )
				{
					return origin + delta * fraction;
				}
			}
			else
			{

				auto inverse = bone_rot;
				inverse.x = -inverse.x;
				inverse.y = -inverse.y;
				inverse.z = -inverse.z;
				const auto local_dir = inverse.rotate_vector( dir );
				const auto extents = ( hitbox.maxs - hitbox.mins ) * 0.5f;
				auto distance = 8192.0f;

				if ( std::fabs( local_dir.x ) > 1.0e-6f ) distance = std::min( distance, std::fabs( extents.x / local_dir.x ) );
				if ( std::fabs( local_dir.y ) > 1.0e-6f ) distance = std::min( distance, std::fabs( extents.y / local_dir.y ) );
				if ( std::fabs( local_dir.z ) > 1.0e-6f ) distance = std::min( distance, std::fabs( extents.z / local_dir.z ) );

				if ( distance < 8192.0f )
				{
					return center + dir * distance;
				}
			}

			return center;
		};

		const auto scaled_surface = [ & ]( const math::vector3& direction )
		{
			const auto surface = surface_point( direction );
			return center + ( surface - center ) * scale;
		};



		const auto dense = scale >= 0.7f;
		const auto forward_dir = shoot_dir;

		switch ( hitbox.index )
		{
		case 0:
		{
			out.reserve( dense ? 8 : 4 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			out.push_back( scaled_surface( up ) );
			out.push_back( scaled_surface( -up ) );
			if ( dense )
			{
				out.push_back( scaled_surface( forward_dir ) );
				out.push_back( scaled_surface( -forward_dir ) );
				out.push_back( scaled_surface( ( right + up ).normalized( ) ) );
				out.push_back( scaled_surface( ( -right + up ).normalized( ) ) );
			}
			break;
		}

		case 2: case 3:
		{
			out.reserve( dense ? 4 : 2 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			if ( dense )
			{
				out.push_back( scaled_surface( forward_dir ) );
				out.push_back( scaled_surface( -forward_dir ) );
			}
			break;
		}

		case 4: case 5: case 6:
		{
			out.reserve( dense ? 5 : 3 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			if ( hitbox.index == 6 || dense )
			{
				out.push_back( scaled_surface( up ) );
			}
			if ( dense )
			{
				out.push_back( scaled_surface( forward_dir ) );
				out.push_back( scaled_surface( -forward_dir ) );
			}
			break;
		}

		case 7: case 8: case 9: case 10: case 11: case 12:
		{




			out.reserve( dense ? 4 : 2 );
			out.push_back( center + ( capsule_a - center ) * scale );
			out.push_back( center + ( capsule_b - center ) * scale );
			if ( dense )
			{
				out.push_back( scaled_surface( right ) );
				out.push_back( scaled_surface( -right ) );
			}
			break;
		}

		case 13: case 14: case 15: case 16: case 17: case 18:
		{
			out.reserve( dense ? 2 : 1 );
			out.push_back( center + ( capsule_b - center ) * scale );
			if ( dense )
			{
				out.push_back( center + ( capsule_a - center ) * scale );
			}
			break;
		}

		default:
		{
			out.reserve( 2 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			break;
		}
		}

		return out;
	}

	bool rage::should_stop_between_shots( ) const noexcept
	{
		if ( !settings::g_combat.m_ragebot.autostop_between_shots.value || this->m_last_shot_tick == 0 )
		{
			return false;
		}

		const auto local = systems::g_local.get( );
		if ( !local.controller )
		{
			return false;
		}

		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		return static_cast< std::uint32_t >( tick_base ) <= this->m_last_shot_tick + 4;
	}

	bool rage::should_stop_movement( const aim_context& ctx ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& prestate = systems::g_prediction.pre( );
		const auto velocity = prestate.networked_velocity;

		if ( shared_ctx.weapon_type == cstypes::weapon_type::sniper && !ctx.is_scoped )
		{
			return false;
		}

		if ( ctx.on_ground )
		{
			const auto speed_2d = velocity.length_2d( );
			if ( speed_2d <= 0.1f )
			{
				return false;
			}

			const auto inaccuracy_move = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyMove"_hash ) );
			const auto inaccuracy_stand = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyStand"_hash ) );

			return speed_2d * inaccuracy_move > inaccuracy_stand;
		}

		if ( shared_ctx.weapon_type != cstypes::weapon_type::sniper )
		{
			return false;
		}

		if ( velocity.z > 140.0f )
		{
			return false;
		}

		const auto sv_gravity = CONVAR ("sv_gravity")->get<float>( );
		const auto sv_friction = CONVAR ("sv_friction")->get<float>( );
		const auto sv_stopspeed = CONVAR ("sv_stopspeed")->get<float>( );

		const auto inac_jump_initial = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpInitial"_hash ) );
		const auto inac_jump_apex = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpApex"_hash ) );
		const auto shootable_threshold = inac_jump_apex + 0.001f;
		const auto early_threshold = inac_jump_initial * 0.55f + inac_jump_apex * 0.45f;
		const auto air_inaccuracy = g_shared.get_air_inaccuracy( velocity.z, inac_jump_initial, inac_jump_apex );

		if ( air_inaccuracy <= shootable_threshold || air_inaccuracy <= early_threshold )
		{
			return true;
		}

		auto sim_vz = velocity.z;
		auto ticks_to_shootable{ 0 };

		for ( auto i = 1; i <= 32; ++i )
		{
			sim_vz -= sv_gravity * cstypes::tick_interval;

			if ( g_shared.get_air_inaccuracy( sim_vz, inac_jump_initial, inac_jump_apex ) <= shootable_threshold )
			{
				ticks_to_shootable = i;
				break;
			}
		}

		if ( ticks_to_shootable == 0 )
		{
			return false;
		}

		const auto speed_2d = velocity.length_2d( );
		const auto max_speed = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flMaxSpeed"_hash ) );
		const auto accurate_threshold = max_speed * 0.34f;

		if ( speed_2d <= accurate_threshold )
		{
			return true;
		}

		auto sim_speed = speed_2d;
		auto ticks_to_stop{ 32 };

		for ( auto i = 1; i <= 32; ++i )
		{
			const auto drop = std::fmaxf( sim_speed, sv_stopspeed ) * sv_friction * cstypes::tick_interval;
			sim_speed -= drop;

			if ( sim_speed <= accurate_threshold )
			{
				ticks_to_stop = i;
				break;
			}
		}

		return ticks_to_shootable <= ticks_to_stop + 2;
	}

	float rage::get_min_damage( const settings::combat::ragebot::weapon_group& config, int target_health, bool override_active ) const
	{




		const auto hp = static_cast< float >( target_health );
		const auto lethal_floor = hp + 1.0f;

		const auto base = override_active
			? static_cast< float >( config.min_damage_override_value )
			: static_cast< float >( config.min_damage );




		return std::min( base, lethal_floor );
	}

	float rage::get_knife_damage( float raw, int armor, float armor_ratio ) const
	{
		if ( armor <= 0 )
		{
			return raw;
		}

		const auto ratio = armor_ratio * 0.5f;
		auto damage_to_health = raw * ratio;
		const auto damage_to_armor = ( raw - damage_to_health ) * 0.5f;

		if ( damage_to_armor > static_cast< float >( armor ) )
		{
			damage_to_health = raw - static_cast< float >( armor ) * 2.0f;
		}

		return std::max( 0.0f, std::floorf( damage_to_health ) );
	}

	systems::tracing::result rage::trace_taser_hit( const math::vector3& origin, const math::vector3& forward, float range, std::uintptr_t target_pawn, std::uintptr_t local_pawn ) const
	{
		const auto end = origin + forward * range;
		const int filter_extras[ ]{ 0, 15 };

		for ( const auto extra : filter_extras )
		{
			const auto filter = extra == 0 ? systems::g_tracing.make_filter( local_pawn, 0x001c1003, 4 ) : systems::g_tracing.make_filter( local_pawn, 0x001c1003, 4, 15 );
			auto result = systems::g_tracing.trace( origin, end, filter );

			if ( ( result.fraction < 1.0f || result.all_solid ) && result.hit_entity == target_pawn )
			{
				return result;
			}

			for ( auto radius = 2.0f; radius <= 4.0f; radius += 2.0f )
			{
				const auto sweep_end = end - forward * radius;
				result = systems::g_tracing.trace_sphere( origin, sweep_end, radius, filter );

				if ( ( result.fraction < 1.0f || result.all_solid ) && result.hit_entity == target_pawn )
				{
					return result;
				}
			}
		}

		systems::tracing::result miss{};
		miss.fraction = 1.0f;
		miss.hit_entity = 0;
		return miss;
	}

	systems::tracing::result rage::trace_knife_hit( const math::vector3& origin, const math::vector3& forward, float reach, std::uintptr_t target_pawn, std::uintptr_t local_pawn ) const
	{
		const auto end = origin + forward * reach;
		const auto knife_filter = systems::g_tracing.make_filter( local_pawn, 0x0c3001, 4 );
		auto result = systems::g_tracing.trace( origin, end, knife_filter );

		if ( ( result.fraction < 1.0f || result.all_solid ) && result.hit_entity == target_pawn )
		{
			return result;
		}

		const auto weapon_filter = systems::g_tracing.make_filter( local_pawn, 0x0c3001, 4, 15 );
		result = systems::g_tracing.trace( origin, end, weapon_filter );

		if ( ( result.fraction < 1.0f || result.all_solid ) && result.hit_entity == target_pawn )
		{
			return result;
		}

		for ( auto radius = 14.0f; radius > 0.0f; radius -= 3.0f )
		{
			const auto sweep_end = end - forward * radius;
			result = systems::g_tracing.trace_sphere( origin, sweep_end, radius, weapon_filter );

			if ( ( result.fraction < 1.0f || result.all_solid ) && result.hit_entity == target_pawn )
			{
				return result;
			}
		}

		result.fraction = 1.0f;
		result.hit_entity = 0;
		return result;
	}

	void rage::update_penetration_crosshair( const systems::local::snapshot& local )
	{
		const auto& cfg = settings::g_combat.m_penetration_crosshair;
		const auto& ctx = g_shared.ctx( );

		if ( !cfg.enabled.value || !ctx.valid || !local.is_alive || local.team < 2
			|| !local.pawn || !ctx.weapon
			|| ctx.weapon_type < cstypes::weapon_type::pistol || ctx.weapon_type > cstypes::weapon_type::lmg )
		{
			this->m_penetration_crosshair_state.store( penetration_crosshair_state::unavailable, std::memory_order_relaxed );
			return;
		}



		const auto eye_pos = g_shared.get_eye_position( local.pawn );
		auto view_angles = systems::g_input.get_view_angles( );
		const auto aim_punch = g_shared.get_aim_punch( local.pawn );
		view_angles.x += aim_punch.x;
		view_angles.y += aim_punch.y;

		math::vector3 forward{};
		math::helpers::angle_vectors_left( view_angles, &forward );

		auto pen_damage{ 0.0f };
		const auto can_pen = g_shared.pen( ).can( eye_pos, forward, pen_damage, local );
		this->m_penetration_crosshair_state.store(
			can_pen ? penetration_crosshair_state::penetrable : penetration_crosshair_state::blocked,
			std::memory_order_relaxed );



		this->m_pen_crosshair_hit_valid = false;
		if ( const auto hit = systems::g_tracing.trace( eye_pos, eye_pos + forward * 8192.0f, local.pawn );
			hit.fraction < 1.0f )
		{
			this->m_pen_crosshair_hit = hit.position;
			this->m_pen_crosshair_normal = hit.normal;
			this->m_pen_crosshair_hit_valid = true;
		}
	}

	void rage::draw_penetration_crosshair( xdraw::draw_list& draw_list ) const
	{
		const auto& cfg = settings::g_combat.m_penetration_crosshair;
		if ( !cfg.enabled.value )
		{
			return;
		}

		const auto state = this->m_penetration_crosshair_state.load( std::memory_order_relaxed );
		const auto local = systems::g_local.get( );
		if ( state == penetration_crosshair_state::unavailable || !local.is_alive || systems::g_local.is_in_cinematic( ) )
		{
			return;
		}

		const auto can_pen = state == penetration_crosshair_state::penetrable;

		const auto& fill = can_pen ? cfg.can_penetrate_fill : cfg.blocked_fill;
		const auto& outline = can_pen ? cfg.can_penetrate_outline : cfg.blocked_outline;
		const auto [ screen_w, screen_h ] = xdraw::viewport_size( );
		const auto sw = static_cast< float >( screen_w );
		const auto sh = static_cast< float >( screen_h );







		static math::vector3 sm_hit{};
		static math::vector3 sm_normal{ 0.0f, 0.0f, 1.0f };
		static float sm_alpha{};
		static bool sm_init{};

		const float dt = std::clamp( xdraw::delta_time( ), 0.0001f, 0.05f );
		const float ease = 1.0f - std::exp( -16.0f * dt );

		if ( !sm_init && this->m_pen_crosshair_hit_valid )
		{
			sm_hit = this->m_pen_crosshair_hit;
			sm_normal = this->m_pen_crosshair_normal;
			sm_init = true;
		}

		if ( this->m_pen_crosshair_hit_valid )
		{



			const auto delta = this->m_pen_crosshair_hit - sm_hit;
			if ( delta.length_sqr( ) > 250.0f * 250.0f )
			{
				sm_hit = this->m_pen_crosshair_hit;
				sm_normal = this->m_pen_crosshair_normal;
			}
			else
			{
				sm_hit = sm_hit + delta * ease;

				auto n_delta = this->m_pen_crosshair_normal - sm_normal;
				if ( n_delta.length_sqr( ) > 0.25f )
				{
					sm_normal = this->m_pen_crosshair_normal;
				}
				else
				{
					sm_normal = sm_normal + n_delta * ease;
				}
			}
		}

		sm_alpha += ( ( this->m_pen_crosshair_hit_valid ? 1.0f : 0.0f ) - sm_alpha ) * ease * 0.6f;

		if ( sm_alpha > 0.02f && sm_init )
		{
			const auto proj = systems::g_view.project_full( sm_hit );
			if ( proj.w > 0.0f && proj.screen.x > -sw && proj.screen.x < sw * 2.0f
				&& proj.screen.y > -sh && proj.screen.y < sh * 2.0f )
			{
				const auto n = sm_normal.length_sqr( ) > 0.5f
					? sm_normal.normalized( )
					: math::vector3{ 0.0f, 0.0f, 1.0f };


				auto ref = std::fabs( n.z ) < 0.9f ? math::vector3{ 0.0f, 0.0f, 1.0f } : math::vector3{ 1.0f, 0.0f, 0.0f };
				const auto u = n.cross( ref ).normalized( );
				const auto v = n.cross( u ).normalized( );



				const auto base = sm_hit + n * 0.5f;
				const auto arm = 5.0f;

				const auto fade = [ & ]( const xdraw::color& c ) -> xdraw::color
					{
						return xdraw::color{ c.r, c.g, c.b, static_cast< std::uint8_t >( c.a * sm_alpha ) };
					};

				const auto project = [ & ]( const math::vector3& p )
					{
						return systems::g_view.project_full( p );
					};

				const auto draw_arm = [ & ]( const math::vector3& dir, float len )
					{
						const auto p0 = project( base );
						const auto p1 = project( base + dir * len );
						if ( p0.w > 0.0f && p1.w > 0.0f )
						{
							draw_list.line( p0.screen.x, p0.screen.y, p1.screen.x, p1.screen.y, fade( outline ), 2.0f );
						}
					};


				draw_arm( u, arm );
				draw_arm( u, -arm );
				draw_arm( v, arm );
				draw_arm( v, -arm );

				const auto c = project( base );
				if ( c.w > 0.0f )
				{
					const auto dot_r = std::clamp( arm * 0.35f * ( sh * 0.5f ) / std::max( 1.0f, c.w ), 1.0f, 12.0f );
					draw_list.circle_filled( c.screen.x, c.screen.y, dot_r, fade( fill ), 10 );
					draw_list.circle( c.screen.x, c.screen.y, dot_r * 1.9f, fade( outline ), 1.2f );
				}

				return;
			}
		}

		const auto cx = std::floorf( sw * 0.5f );
		const auto cy = std::floorf( sh * 0.5f );
		constexpr auto half_size{ 3.0f };
		constexpr auto outline_size{ 1.0f };

		if ( cfg.glow )
		{
			auto& glow = xdraw::get_glow( );
			const auto glow_a = static_cast< std::uint8_t >( static_cast< float >( outline.value.a ) * cfg.glow_strength );
			const auto glow_col = xdraw::color{ outline.value.r, outline.value.g, outline.value.b, glow_a };

			glow.rect_filled( cx - half_size - outline_size, cy - half_size - outline_size,
				( half_size + outline_size ) * 2.0f, ( half_size + outline_size ) * 2.0f, glow_col );
		}

		draw_list.rect_filled( cx - half_size - outline_size, cy - half_size - outline_size,
			( half_size + outline_size ) * 2.0f, ( half_size + outline_size ) * 2.0f, outline );
		draw_list.rect_filled( cx - half_size, cy - half_size, half_size * 2.0f, half_size * 2.0f, fill );
	}

}
