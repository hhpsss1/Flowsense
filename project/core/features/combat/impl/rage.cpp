#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <utilities/threadpool/threadpool.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/rendering/rendering.hpp>
#include <protection/game_addresses.hpp>
namespace features::combat {

	// Targets moving faster than this (units/s) between their newest poses are
	// treated as peeking and get their backtrack window shrunk to the newest
	// records so shots track the target's current pose instead of a stale one.
	constexpr auto k_peek_speed_threshold{ 160.0f };

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

		// Safe mode disables ragebot entirely; only visuals and movement remain.
		if ( settings::g_misc.safe_mode.value )
		{
			this->m_should_stop = false;
			this->m_firing_this_tick = false;
			this->m_revolver_cock_ticks = 0;
			return;
		}

		this->m_should_stop = false;
		this->m_firing_this_tick = false;

		// Rotate multipoint angles (vendetta-style) for better coverage against AA
		this->m_head_angle += 2.0f * std::numbers::pi_v<float> / this->k_head_steps;
		if ( this->m_head_angle >= 2.0f * std::numbers::pi_v<float> )
			this->m_head_angle = 0.0f;
		this->m_other_z_step += 1.0f / this->k_other_z_steps;
		if ( this->m_other_z_step >= 1.0f )
			this->m_other_z_step = 0.0f;

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
			diag::set_exception_phase( "rage: run_knife" );

			if ( !g_shared.can_shoot( cmd, local.controller ) )
			{
				return;
			}

			this->run_knife( cmd, aim_ctx, local );
		}
		else if ( is_taser )
		{
			diag::set_exception_phase( "rage: run_taser" );

			if ( !g_shared.can_shoot( cmd, local.controller ) )
			{
				return;
			}

			this->run_taser( cmd, aim_ctx, local );
		}
		else if ( ctx.item_def_idx == cstypes::item_definition_index::weapon_r8_revolver )
		{
			diag::set_exception_phase( "rage: auto_revolver" );
			this->auto_revolver( cmd, aim_ctx, local );
		}
		else
		{
			this->m_revolver_cock_ticks = 0;

			if ( !g_shared.can_shoot( cmd, local.controller ) )
			{
				return;
			}

			diag::set_exception_phase( "rage: run_gun" );
			this->run_gun( cmd, aim_ctx, local );

			diag::set_exception_phase( "rage: autoscope" );
			this->apply_autoscope( cmd, local );

			diag::set_exception_phase( "rage: rapid_fire" );
			this->apply_rapid_fire( cmd );
		}
	}

	void rage::on_render( xdraw::draw_list& draw_list )
	{
		this->draw_penetration_crosshair( draw_list );
		this->draw_doubletap_indicator( draw_list );

		const auto& config = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type );
		if ( !config.visualize_aimbot.value )
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
				col = { 255, 80,  80  }; break; // head — red
			case 2: case 3:
				col = { 220, 220, 60  }; break; // stomach — yellow
			case 4: case 5: case 6:
				col = { 255, 160, 60  }; break; // chest — orange
			case 7: case 8: case 9: case 10: case 11: case 12:
				col = { 80,  160, 255 }; break; // legs — blue
			case 13: case 14: case 15: case 16: case 17: case 18:
				col = { 180, 80,  255 }; break; // arms — purple
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

		// The snapshot pawn can be torn down between the pipeline snapshot and
		// this call (death/respawn), and the reads below dereference it raw.
		if ( !local.pawn )
		{
			return aim_context{};
		}

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

		// A recent landing leaves the jump inaccuracy decaying for a few ticks
		// even though FL_ONGROUND is already set, and the engine accuracy read
		// above still reflects the pre-tick state. Clamp the predicted accuracy
		// with a decay model of that window - otherwise the scan treats the pose
		// as standing and dumps the shot into spread right after landing.
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

	void rage::update_dt_charge( const systems::local::snapshot& local )
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.doubletap.value || !local.controller || !shared_ctx.weapon )
		{
			this->m_dt_charge_start = -1;
			this->m_dt_ready = false;
			this->m_dt_shot_count = 0;
			return;
		}

		// Charge is measured against the weapon cycle time: after a doubletap
		// shot the weapon refuses to fire for a full cycle, so the charge is
		// complete only when the weapon is ready to shoot again.
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		const auto next_primary = memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash ) );

		if ( this->m_dt_charge_start <= 0 )
		{
			// No doubletap fired in this cycle: charged whenever the weapon is ready.
			this->m_dt_ready = next_primary <= tick_base;
			return;
		}

		// The weapon's next-primary tick was stamped at the last shot, so the
		// difference against that shot tick is the full cycle in ticks.
		const auto cycle_ticks = std::clamp( next_primary - this->m_last_shot_tick_when, 1, 64 );
		const auto elapsed = tick_base - this->m_dt_charge_start;

		this->m_dt_ready = elapsed >= cycle_ticks;
		if ( this->m_dt_ready )
		{
			this->m_dt_charge_start = -1;
		}
	}

	float rage::get_dt_charge_progress( ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.doubletap.value || this->is_dt_ready( ) )
		{
			return this->is_dt_ready( ) ? 1.0f : 0.0f;
		}

		if ( this->m_dt_charge_start <= 0 )
		{
			return 1.0f;
		}

		const auto local = systems::g_local.get( );
		if ( !local.controller || !shared_ctx.weapon )
		{
			return 0.0f;
		}

		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		const auto next_primary = memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash ) );
		const auto cycle_ticks = std::clamp( next_primary - this->m_last_shot_tick_when, 1, 64 );

		return std::clamp( static_cast< float >( tick_base - this->m_dt_charge_start ) / static_cast< float >( cycle_ticks ), 0.0f, 1.0f );
	}

	bool rage::process_doubletap( systems::input::usercmd* cmd, const systems::local::snapshot& local, bool charge_dt )
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.doubletap.value )
		{
			return false;
		}

		const auto base_cmd = cmd->csgo_user_cmd.mutable_base( );
		if ( !base_cmd )
		{
			return false;
		}

		this->update_dt_charge( local );

		const auto client_tick = base_cmd->client_tick( );
		const auto next_primary = memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash ) );
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );

		const auto can_attack = client_tick >= next_primary && tick_base >= g_shared.last_shoot_tick( ) + 2 && !memory::read<bool>( shared_ctx.weapon + SCHEMA( "C_CSWeaponBase", "m_bInReload"_hash ) ) && memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_iClip1"_hash ) ) > 0;

		const auto should_attack = can_attack && charge_dt && this->is_dt_ready( );

		if ( should_attack )
		{
			if ( const auto subtick_moves = base_cmd->mutable_subtick_moves( ) )
			{
				const auto old_size = subtick_moves->m_current_size;
				const auto press = systems::g_input.acquire_subtick_step( subtick_moves );
				const auto release = systems::g_input.acquire_subtick_step( subtick_moves );
				if ( press && release )
				{
					press->set_button( cstypes::command_buttons::in_attack );
					press->set_pressed( true );
					press->set_when( 0.0f );
					press->set_analog_forward_delta( 0.0f );
					press->set_analog_left_delta( 0.0f );

					release->set_button( cstypes::command_buttons::in_attack );
					release->set_pressed( false );
					release->set_when( std::nextafter( 1.0f, 0.0f ) );
					release->set_analog_forward_delta( 0.0f );
					release->set_analog_left_delta( 0.0f );

					// Input history fixes for the doubletap shot: stale player
					// ticks would make the server try to run the shot on an
					// unrelated historical tick. Zero them all and repoint the
					// first entry at the current command state.
					const auto history_size = cmd->csgo_user_cmd.input_history_size( );
					for ( auto i = 0; i < history_size; ++i )
					{
						const auto entry = cmd->csgo_user_cmd.mutable_input_history( i );
						if ( !entry )
						{
							continue;
						}

						entry->set_player_tick_count( 0 );
						entry->set_player_tick_fraction( 0.0f );
					}

					if ( history_size > 0 )
					{
						if ( const auto entry = cmd->csgo_user_cmd.mutable_input_history( 0 ) )
						{
							const auto view_angles = systems::g_input.get_view_angles( );
							const auto aim_punch = g_shared.get_aim_punch( local.pawn );

							if ( const auto angles = entry->mutable_view_angles( ) )
							{
								angles->set_x( view_angles.x - aim_punch.x );
								angles->set_y( view_angles.y - aim_punch.y );
							}

							if ( const auto shoot = entry->mutable_shoot_position( ) )
							{
								const auto eye = g_shared.get_eye_position( local.pawn );
								shoot->set_x( eye.x );
								shoot->set_y( eye.y );
								shoot->set_z( eye.z );
							}

							entry->set_render_tick_count( 0 );
							entry->set_render_tick_fraction( 0.0f );
						}
					}

					// The shot drained the charge; the next doubletap must
					// wait for the weapon cycle again. The shot counter keeps
					// consecutive doubletaps alternating their tick shift.
					this->m_dt_charge_start = tick_base;
					this->m_dt_ready = false;
					++this->m_dt_shot_count;

					// The subtick release is the final state. Do not let the
					// base command turn this pulse into a held attack.
					cmd->buttons.value &= ~cstypes::command_buttons::in_attack;
					cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
					cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_attack;
					cmd->csgo_user_cmd.set_attack1_start_history_index( -1 );
					return true;
				}

				// Do not leave a partial press in the command if allocation of
				// the matching release failed.
				subtick_moves->m_current_size = old_size;
			}
		}

		cmd->buttons.value &= ~cstypes::command_buttons::in_attack;
		cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
		cmd->buttons.value_scroll &= ~cstypes::command_buttons::in_attack;
		cmd->csgo_user_cmd.set_attack1_start_history_index( -1 );
		return false;
	}

	void rage::run_double_tap( systems::input::usercmd* cmd )
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.doubletap.value )
		{
			return;
		}

		if ( !cmd || !cmd->csgo_user_cmd.has_base( ) || !shared_ctx.weapon )
		{
			return;
		}

		this->m_dt_ready = true;

		// The ragebot already stamped its shot against the backtrack record
		// for the current tick. Re-pointing those entries at the weapon's
		// earliest legal tick makes the server evaluate the hit against an
		// unrelated older pose, which turns ragebot+doubletap into a whiff.
		if ( this->m_firing_this_tick )
		{
			return;
		}

		auto* history = cmd->csgo_user_cmd.mutable_input_history( );
		if ( !history || !history->m_rep )
		{
			return;
		}

		const auto allocated = history->m_rep->allocated_size;

		if ( cmd->buttons.value & cstypes::command_buttons::in_attack )
		{
			for ( auto i = 0; i < allocated; ++i )
			{
				const auto entry = history->mutable_at( i );
				if ( !entry )
				{
					continue;
				}

				entry->set_player_tick_count( 0 );
				entry->set_render_tick_count( 0 );
			}
		}

		float wat_tick_offset_int{};
		const auto tick_ratio = memory::read<float>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_flNextPrimaryAttackTickRatio"_hash ) );
		const auto wat_tick_offset = memory::read<float>( shared_ctx.weapon + SCHEMA( "C_CSWeaponBase", "m_flWatTickOffset"_hash ) );

		auto temp = tick_ratio + std::modff( wat_tick_offset, &wat_tick_offset_int );
		auto player_tick = static_cast< int >( wat_tick_offset_int ) + memory::read<int>( shared_ctx.weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash ) );

		if ( temp >= 1.0f )
		{
			++player_tick;
		}
		else if ( temp < 0.0f )
		{
			--player_tick;
		}

		if ( cmd->csgo_user_cmd.attack1_start_history_index( ) > -1 )
		{
			++this->m_dt_shot_count;
		}

		for ( auto i = 0; i < allocated; ++i )
		{
			const auto entry = history->mutable_at( i );
			if ( !entry )
			{
				continue;
			}

			entry->set_player_tick_count( player_tick - 1 );
			entry->set_player_tick_fraction( 0.0f );
		}

		cmd->csgo_user_cmd.set_attack1_start_history_index( -1 );
	}

	void rage::apply_rapid_fire( systems::input::usercmd* cmd )
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.rapid_fire.value )
		{
			return;
		}

		if ( ( cmd->buttons.value & cstypes::command_buttons::in_attack ) == 0 )
		{
			return;
		}

		// While attack is held every input-history entry must resolve on the
		// current tick, otherwise the server staggers the burst over history.
		const auto history_size = cmd->csgo_user_cmd.input_history_size( );
		for ( auto i = 0; i < history_size; ++i )
		{
			const auto entry = cmd->csgo_user_cmd.mutable_input_history( i );
			if ( !entry )
			{
				continue;
			}

			entry->set_player_tick_count( 0 );
			entry->set_player_tick_fraction( 0.0f );
			entry->set_render_tick_count( 0 );
			entry->set_render_tick_fraction( 0.0f );
		}
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

		// Wait until the shot has resolved on the server before judging it.
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		if ( tick_base - this->m_last_shot_tick_when < 2 )
		{
			return;
		}

		const auto hp = memory::read<int>( this->m_last_shot_pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
		const auto damage_dealt = this->m_last_shot_health - hp;

		if ( damage_dealt >= 1 )
		{
			// The shot connected: the adaptive boost is no longer needed.
			this->m_adaptive_hc_boost = 0.0f;
			this->m_miss_streak = 0;
		}
		else
		{
			// Missed. Every streak of misses relaxes the hitchance a step so
			// the bot keeps shooting instead of staring at an unshootable shot.
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

		// Already holding a pawn: keep waiting while it is still alive and the
		// hold budget has not run out. The actual hold bookkeeping lives in
		// run_gun; this decides whether waiting is still worthwhile.
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

		// No hold yet: extrapolate each candidate a few ticks ahead and check
		// whether a near-future pose exposes a lethal headshot. If it does, the
		// current non-lethal shot is worth delaying until that pose arrives.
		const auto lookahead = std::clamp( config.shot_timing_lookahead.value, 1, 32 );
		const auto eye = g_shared.get_shoot_position( );

		for ( auto& cand : candidates )
		{
			if ( !cand.pawn )
			{
				continue;
			}

			// Backwards anti-aim: the head capsule is edge-on and unreliable, so
			// a "future lethal head" will never become shootable. Never delay the
			// current body shot waiting for one - bodytap instead.
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
					// Do not wait on an unshootable head for backwards anti-aim.
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

	bool rage::is_enemy_aiming_at_us( std::uintptr_t pawn, const math::vector3& local_eye ) const
	{
		if ( !pawn )
		{
			return false;
		}

		const auto origin = memory::read<math::vector3>( pawn + SCHEMA( "C_BaseEntity", "m_vecAbsOrigin"_hash ) );
		const auto view_offset = memory::read<math::vector3>( pawn + SCHEMA( "C_BaseModelEntity", "m_vecViewOffset"_hash ) );
		const auto eye_pos = origin + view_offset;

		const auto eye_angles = memory::read<math::vector3>( pawn + SCHEMA( "C_CSPlayerPawn", "m_angEyeAngles"_hash ) );

		math::vector3 forward{};
		math::helpers::angle_vectors_left( eye_angles, &forward );

		const auto to_us = ( local_eye - eye_pos ).normalized( );
		return forward.dot( to_us ) > 0.985f;
	}

	float rage::get_target_priority( std::uintptr_t pawn, const math::vector3& local_eye, const systems::local::snapshot& local ) const
	{
		if ( !pawn )
		{
			return 0.0f;
		}

		const auto origin = memory::read<math::vector3>( pawn + SCHEMA( "C_BaseEntity", "m_vecAbsOrigin"_hash ) );
		const auto dist = ( origin - local_eye ).length( );
		const auto dist_score = std::clamp( 1.0f - dist / 3000.0f, 0.0f, 1.0f );

		const auto aiming_score = this->is_enemy_aiming_at_us( pawn, local_eye ) ? 1.0f : 0.0f;

		const auto health = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) );
		const auto hp_score = std::clamp( 1.0f - static_cast< float >( health ) / 100.0f, 0.0f, 1.0f );

		return dist_score * 0.45f + aiming_score * 0.35f + hp_score * 0.20f;
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

			// Distance from the teammate origin to the shot segment.
			const auto pos = memory::read<math::vector3>( pawn + SCHEMA( "C_BaseEntity", "m_vecAbsOrigin"_hash ) );
			const auto rel = pos - eye;
			const auto t = std::clamp( ( rel.x * line.x + rel.y * line.y + rel.z * line.z ) / line_len_sq, 0.0f, 1.0f );
			const auto closest = eye + line * t;
			const auto dx = pos.x - closest.x;
			const auto dy = pos.y - closest.y;
			const auto dz = pos.z - closest.z;
			const auto dist_sq = dx * dx + dy * dy + dz * dz;

			if ( dist_sq < 28.0f * 28.0f )
			{
				return false;
			}
		}

		return true;
	}

	void rage::draw_doubletap_indicator( xdraw::draw_list& draw_list ) const
	{
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		if ( !config.doubletap.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || !local.pawn || systems::g_local.is_in_cinematic( ) )
		{
			return;
		}

		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;

		constexpr auto margin{ 10.0f };
		constexpr auto header_h{ hud::caption_h };
		constexpr auto body_h{ 58.0f };
		constexpr auto pad_x{ 8.0f };
		constexpr auto text_nudge{ 0.5f };

		static animation::fade container_alpha;
		container_alpha.fade_in( 0.2f );
		container_alpha.update( );
		const auto master_alpha = container_alpha.alpha( );
		const auto master_u8 = hud::fade_alpha( master_alpha );

		const auto [header_tw, header_th] = xdraw::measure_text( "doubletap" );

		// Flat panel sized to the ring, caption on the top border.
		constexpr auto ring_diameter{ 40.0f };
		const auto panel_w = std::max( header_tw + pad_x * 2.0f + hud::caption_gap * 2.0f + tokens::pad * 2.0f, ring_diameter + pad_x * 2.0f + tokens::pad * 2.0f );
		const auto panel_h = header_h + body_h;

		auto& dt_x = settings::g_misc.m_widgets.doubletap_x.value;
		auto& dt_y = settings::g_misc.m_widgets.doubletap_y.value;

		float x = dt_x >= 0.0f ? dt_x : std::max( margin, rendering::g_menu.pos_x( ) - panel_w - margin );
		float y = dt_y >= 0.0f ? dt_y : std::max( margin, rendering::g_menu.pos_y( ) + margin + header_h );

		const auto& input = xui::ctx( ).input;
		const auto header_rect = xui::rect{ x, y, panel_w, header_h };

		static bool dragging{};
		static float grab_dx{}, grab_dy{};

		if ( !dragging && input.mouse_clicked && header_rect.contains( input.mouse_x, input.mouse_y ) )
		{
			dragging = true;
			grab_dx = input.mouse_x - x;
			grab_dy = input.mouse_y - y;
		}

		if ( dragging )
		{
			if ( input.mouse_down )
			{
				x = std::clamp( input.mouse_x - grab_dx, 0.0f, static_cast< float >( screen_w ) - panel_w );
				y = std::clamp( input.mouse_y - grab_dy, 0.0f, static_cast< float >( screen_h ) - panel_h );

				dt_x = x;
				dt_y = y;
			}
			else
			{
				dragging = false;
			}
		}

		const auto body_y = y + header_h;

		hud::surface( draw_list, x, body_y, panel_w, body_h, master_alpha, true );
		hud::caption( draw_list, x, y, panel_w, "doubletap", master_alpha );
		hud::accent_line( draw_list, x, y + panel_h - 1.0f, panel_w, 1.0f, master_alpha );

		const auto progress = this->get_dt_charge_progress( );
		const auto ready = this->is_dt_ready( );
		// Ready reads in the accent colour, charging in plain text - same
		// two-colour language the menu uses instead of a separate green/red.
		const auto fill = ready ? tokens::col_accent : tokens::col_text_dim;
		const auto track = tokens::col_border;

		const auto cx = x + panel_w * 0.5f;
		const auto cy = body_y + body_h * 0.5f - 4.0f;
		constexpr auto ring_radius{ 17.0f };
		constexpr auto ring_segments{ 64 };

		draw_list.circle( cx, cy, ring_radius, track.alpha( master_u8 ), 3.0f, ring_segments );

		if ( progress > 0.01f )
		{
			const auto filled = static_cast< int >( static_cast< float >( ring_segments ) * std::clamp( progress, 0.0f, 1.0f ) );
			std::vector<float> arc;
			arc.reserve( static_cast< std::size_t >( filled + 1 ) * 2 );

			for ( auto i = 0; i <= filled; ++i )
			{
				const auto a = -std::numbers::pi_v<float> * 0.5f + static_cast< float >( i ) / static_cast< float >( ring_segments ) * std::numbers::pi_v<float> * 2.0f;
				arc.push_back( cx + std::cosf( a ) * ring_radius );
				arc.push_back( cy + std::sinf( a ) * ring_radius );
			}

			draw_list.polyline( arc, fill.alpha( master_u8 ), false, 3.0f );
		}

		constexpr auto badge_radius{ 11.0f };
		draw_list.circle_filled( cx, cy, badge_radius, tokens::col_card.alpha( master_u8 ), 48 );

		// Lightning bolt icon inside the badge.
		const auto bolt_scale = badge_radius * 0.72f;
		const std::vector<float> bolt{
			cx + bolt_scale * 0.15f, cy - bolt_scale,
			cx + bolt_scale * 0.45f, cy - bolt_scale * 0.25f,
			cx + bolt_scale * 0.10f, cy - bolt_scale * 0.25f,
			cx - bolt_scale * 0.15f, cy + bolt_scale,
			cx - bolt_scale * 0.45f, cy + bolt_scale * 0.25f,
			cx - bolt_scale * 0.10f, cy + bolt_scale * 0.25f,
		};
		draw_list.polyline( bolt, fill.alpha( master_u8 ), true, 2.5f );

		const auto status = ready ? xs( "charged" ) : xs( "charging" );
		const auto [tw, th] = xdraw::measure_text( status );
		const auto status_col = ready ? fill : tokens::col_text_faint;
		draw_list.text( std::floor( cx - tw * 0.5f ), std::floor( body_y + body_h - th - 6.0f ), status, status_col.alpha( master_u8 ) );
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
					// No usable lag records and extrapolation refuses (e.g. the
					// target is standing completely still or the extrapolation
					// budget is exceeded). The enemy is still alive and shootable,
					// so snapshot a live record from the current scene skeleton
					// instead of silently dropping the candidate -- otherwise a
					// stationary opponent (very common with backwards anti-aim)
					// can never be engaged.
					shared::lagcomp::record live{};
					if ( !live.setup( pawn ) )
					{
						continue;
					}

					live.extrapolated = true;
					const_cast<rage*>( this )->m_extrapolated_records.push_back( std::move( live ) );
					records.push_back( &const_cast<rage*>( this )->m_extrapolated_records.back( ) );
				}
				else
				{
					const_cast<rage*>( this )->m_extrapolated_records.push_back( std::move( *extrap ) );
					records.push_back( &const_cast<rage*>( this )->m_extrapolated_records.back( ) );
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

			// Movement speed between the two newest poses. Fast movers (peeking
			// enemies) get a shrunk backtrack window below: an old pose is stale
			// by the time the shot resolves, and shooting it is exactly how
			// backtrack shots end up missing a peek.
			{
				auto target_speed{ 0.0f };
				if ( records.size( ) > 1 )
				{
					const auto dt_ticks = std::max( 1, records[ 0 ]->tick - records[ 1 ]->tick );
					target_speed = ( records[ 0 ]->origin - records[ 1 ]->origin ).length( ) / static_cast< float >( dt_ticks ) * 64.0f;
				}

				c.speed = target_speed;
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

						// Newest record first: it is the most reliable pose and the
						// scan loop exits early when it yields a guaranteed kill.
						add_index( 0 );

						if ( count > 1 )
						{
							// A target that is actively moving must be engaged on its
							// current pose. Deep backtracking into stale poses on a
							// fast mover is what makes peek shots miss - limit such
							// targets to the newest two records.
							if ( c.speed > k_peek_speed_threshold )
							{
								add_index( 1 );
							}
							else
							{
								const auto budget = k_max_scan_records - picked;
								const auto step = std::max( 1, ( static_cast< int >( count ) - 1 + budget - 1 ) / budget );

								for ( auto idx = step; idx < static_cast< int >( count ) - 1; idx += step )
								{
									add_index( idx );
								}

								// Oldest record extends the backtrack window furthest.
								add_index( static_cast< int >( count - 1 ) );
							}
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

				if ( config.target_priority.value )
				{
					c.priority = this->get_target_priority( pawn, g_shared.get_eye_position( local.pawn ), local );
				}
			}

			out.push_back( c );
		}

		return out;
	}

	void rage::run_gun( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local, bool allow_fire )
	{
		this->update_miss_tracking( );

		if ( !settings::g_combat.m_ragebot.enabled )
		{
			return;
		}

		auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );

		// Doubletap is handled by run_double_tap at the end of create_move:
		// the shot command and its input history entries are rewritten there,
		// so no extra press/release subtick steps are injected from the ragebot.
		const auto finish_doubletap = [ & ]( bool )
		{
			return false;
		};

		// Only trace against enemies the current weapon can actually reach.
		// This removes a big chunk of per-frame autowall work when a fight is
		// happening at typical combat range.
		const auto scan_range = shared_ctx.range > 0.0f ? shared_ctx.range : 8192.0f;
		auto candidates = this->gather_candidates( local, scan_range * scan_range );

		{
			std::lock_guard lock( m_debug_mtx );
			m_debug_points.clear( );
		}

		if ( candidates.empty( ) )
		{
			finish_doubletap( false );
			return;
		}

		// In-air autostop: if we're in the air with a valid enemy nearby and the
		// weapon will become shootable during descent, stop movement now.
		{
			const auto& air_pre = systems::g_prediction.pre();
			if ( !( air_pre.flags & cstypes::entity_flags::on_ground )
				&& config.autostop_inair.value
				&& settings::g_combat.m_ragebot.autostop.value )
			{
				// Quick check: is there actually an enemy within weapon range?
				// Use a reasonable range to avoid false positives from distant enemies
				const auto scan_range = shared_ctx.range > 0.0f ? shared_ctx.range : 400.0f;
				const auto scan_range_sq = scan_range * scan_range;
				const auto players = systems::g_entities.get_by_type( systems::entities::type::player );
				auto has_target = false;
				for ( const auto& p : players )
				{
					if ( !p.ptr || p.ptr == local.controller )
						continue;
					const auto pawn_handle = memory::read<std::uint32_t>( p.ptr + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
					const auto pawn = systems::g_entities.lookup( pawn_handle );
					if ( !pawn || pawn == local.pawn )
						continue;
					const auto team = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
					if ( !local.is_this_other_team( team ) )
						continue;
					if ( !memory::read<bool>( p.ptr + SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash ) ) )
						continue;
					const auto origin = memory::read<math::vector3>( pawn + SCHEMA( "C_BaseEntity", "m_vecAbsOrigin"_hash ) );
					const auto delta = origin - air_pre.origin;
					if ( delta.length_sqr() < scan_range_sq )
					{
						has_target = true;
						break;
					}
				}

				if ( has_target )
				{
					const auto inac_jump_initial = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpInitial"_hash ) );
					const auto inac_jump_apex = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpApex"_hash ) );
					const auto sv_gravity = CONVAR( "sv_gravity" )->get<float>( );
					const auto air_inaccuracy = g_shared.get_air_inaccuracy( air_pre.networked_velocity.z, inac_jump_initial, inac_jump_apex );
					const auto shootable_threshold = inac_jump_apex + 0.001f;

					if ( air_inaccuracy > shootable_threshold )
					{
						auto sim_vz = air_pre.networked_velocity.z;
						for ( auto i = 1; i <= 32; ++i )
						{
							sim_vz -= sv_gravity * cstypes::tick_interval;
							const auto sim_inac = g_shared.get_air_inaccuracy( sim_vz, inac_jump_initial, inac_jump_apex );
							if ( sim_inac <= shootable_threshold )
							{
								this->m_should_stop = true;
								break;
							}
							if ( sim_vz < -sv_gravity * cstypes::tick_interval * 2 )
								break;
						}
					}
				}
			}
		}

		auto eye_candidates = g_shared.sh( ).get_candidates( );
		if ( eye_candidates.count == 0 )
		{
			eye_candidates.entries[ 0 ].position = g_shared.get_shoot_position( );
			eye_candidates.entries[ 0 ].is_uninterpolated = true;
			eye_candidates.count = 1;
		}

		const auto scan_from_eye_candidates = [ & ]( const math::vector3& eye_offset, float inaccuracy )
		{
			std::vector<scan_hit> hits_out;

			for ( auto i = 0; i < eye_candidates.count; ++i )
			{
				const auto eye = eye_candidates.entries[ i ].position + eye_offset;
				auto hits = this->scan_players( eye, inaccuracy, ctx, candidates, local );
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
			auto all_hits = scan_from_eye_candidates( {}, shared_ctx.inaccuracy );

			if ( all_hits.empty( ) )
			{
				finish_doubletap( false );
				return;
			}

			auto best = this->select_best( ctx, all_hits, shared_ctx.inaccuracy );
			if ( !best.valid )
			{
				finish_doubletap( false );
				return;
			}

			if ( !allow_fire )
			{
				return;
			}

			const auto subtick_attack = finish_doubletap( true );
			this->fire_gun( cmd, best, false, best.hit.source_eye.position, local, subtick_attack );
			return;
		}

		const auto primary_eye = eye_candidates.entries[ 0 ].position;
		const auto& prestate = systems::g_prediction.pre( );

		// Current-shot selection is always based on current engine shoot-history.
		auto current_hits = scan_from_eye_candidates( {}, ctx.predicted_inaccuracy );
		auto best = this->select_best( ctx, current_hits, ctx.predicted_inaccuracy );

		const auto needed_hc = config.hitchance_override.value ? static_cast< float >( config.hitchance_override_value ) / 100.0f : static_cast< float >( config.hitchance ) / 100.0f;
		const auto duckpeek_active = settings::g_combat.m_duckpeek.enabled.value && ctx.on_ground;
		const auto is_ducked = ( prestate.flags & cstypes::entity_flags::ducking ) != 0;

		const auto standing_inaccuracy = duckpeek_active ? this->get_standing_inaccuracy( local, ctx ) : ctx.predicted_inaccuracy;
		const auto standing_hc = best.valid
			? ( duckpeek_active ? this->evaluate_hitchance( best.hit, ctx, standing_inaccuracy ) : best.hitchance )
			: 0.0f;

		const auto accurate = best.valid && standing_hc >= needed_hc;
		const auto max_acc = g_shared.is_max_accuracy( standing_inaccuracy );

		// The weapon keeps recoil and the shots-fired accuracy penalty for a few
		// ticks after a shot. Forcing a second shot inside that window just dumps
		// it into spread - the classic second-shot whiff on the same target.
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		const auto recoil_recovering = this->m_last_shot_tick != 0 && static_cast< std::uint64_t >( tick_base ) <= static_cast< std::uint64_t >( this->m_last_shot_tick ) + 3;

		const auto force = best.valid && !recoil_recovering && ( ctx.on_ground ? ( config.force_shot.value && max_acc ) : ( config.force_shot_air.value && max_acc ) );

		// Backwards anti-aim faces the target's model away from us, so every
		// hitbox is edge-on to the bullet line. That drags listing get_each
		// hit far below the configured hitchance even on an exposed head or
		// crouched chest, which leaves selects picking a hit that never passes
		// the gate and the bot staring at the peek without firing. For these
		// targets trust the scan itself and force the best committed hit —
		// a lethal headshot when one exists, otherwise a body shot that at
		// least meets min damage — instead of refusing to shoot entirely.
		const auto backwards_force = [ & ]( ) -> bool
		{
			// Only force-fire when the local is itself at max accuracy
			// (standing still / noscoped in / no spread). Forcing while
			// strafing just dumps bullets into spread and whiffs.
			if ( !max_acc || recoil_recovering || !best.valid || !best.hit.meets_min_damage || !best.hit.record || !best.hit.record->valid )
			{
				return false;
			}

			// Never force a low-probability shot unless it actually kills:
			// a sub-gate body hit that only does 51 damage whiffs far more
			// often than it trades, which is the "missing shots a lot" bug.
			// The non-lethal floor rides the configured hitchance, not a
			// hardcoded value.
			const auto lethal = best.hit.damage >= static_cast< float >( best.hit.health );
			if ( !lethal && best.hitchance < needed_hc - 0.10f )
			{
				return false;
			}

			const auto& rec = best.hit.record;
			const auto eye = best.hit.source_eye.position.length_sqr( ) > 0.1f
				? best.hit.source_eye.position
				: g_shared.get_shoot_position( );
			const auto angle_to_target = math::helpers::calculate_angle( eye, rec->origin );
			return std::fabsf( math::helpers::normalize_yaw( rec->rotation.y - angle_to_target.y ) ) > 100.0f;
		}( );

		const auto shot_viable = accurate || force || backwards_force;

		// Commit fallback: the scan verified a real hit on the current best
		// target, but the pose keeps the estimated hitchance below the gate
		// (backwards AA). Never let that stall the fight while we're actually
		// on target — fire the highest-damage hit we already committed to.
		// This mirrors the reference scan which always engages its best-damage
		// hit once the local is at max accuracy. While strafing (spread > max
		// accuracy) we must NOT force: that is exactly what causes whiffs.
		const auto lethal = best.hit.damage >= static_cast< float >( best.hit.health );
		const auto committed = !recoil_recovering && max_acc && best.valid && best.hit.record && best.hit.record->valid && best.hit.meets_min_damage && !accurate && ( lethal || best.hitchance >= needed_hc - 0.10f );
		const auto shootable = shot_viable || committed;
		if ( !shootable && this->should_stop_movement( ctx ) )
		{
			const auto stop = this->predict_stop( ctx, primary_eye, local );
			if ( stop )
			{
				const auto future_offset = stop->eye - primary_eye;
				auto planned_hits = scan_from_eye_candidates( future_offset, stop->inaccuracy );
				const auto planned = this->select_best( ctx, planned_hits, stop->inaccuracy );
				this->m_should_stop = planned.valid;
			}
			else
			{
				this->m_should_stop = best.valid;
			}
		}

		if ( !best.valid )
		{
			finish_doubletap( false );
			return;
		}

		if ( duckpeek_active && allow_fire )
		{
			if ( shootable )
			{
				this->m_release_duck_for_shot = true;
			}
			else if ( !this->m_duckpeek_reduck )
			{
				this->m_release_duck_for_shot = false;
			}
		}

		auto ready_to_fire = shootable;
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

		// Safe line: never wallbang through a teammate standing in the bullet line.
		if ( config.safe_line_check.value && best.hit.penetrated && !this->safe_line_clear( best.hit.source_eye.position, best, local ) )
		{
			finish_doubletap( false );
			return;
		}

		// Shot timing: when the current best shot is not lethal but a near-future
		// pose exposes a kill, hold the shot (and autostop) until that pose arrives.
		if ( config.shot_timing.value )
		{
			this->m_timing_records.clear( );
			const auto timing = this->evaluate_shot_timing( ctx, candidates, best, local );
			if ( timing.should_wait && ready_to_fire )
			{
				if ( this->m_hold_pawn == best.hit.pawn && this->m_hold_ticks >= config.shot_timing_max_hold.value )
				{
					// Hold budget exhausted: take the shot anyway.
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
			const auto subtick_attack = finish_doubletap( true );
			this->fire_gun( cmd, best, !accurate && ( force || backwards_force || committed ), best.hit.source_eye.position, local, subtick_attack );

			if ( duckpeek_active )
			{
				this->m_duckpeek_reduck = true;
				this->m_release_duck_for_shot = false;
			}
		}
		else
		{
			finish_doubletap( false );
		}
	}

	void rage::run_taser( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local )
	{
		if ( !settings::g_combat.m_zeusbot.enabled )
		{
			return;
		}

		const auto shared_ctx_range = g_shared.ctx( ).range;
		const auto taser_range = shared_ctx_range > 0.0f ? shared_ctx_range : 8192.0f;
		auto candidates = this->gather_candidates( local, taser_range * taser_range );
		if ( candidates.empty( ) )
		{
			return;
		}

		auto eye_candidates = g_shared.sh( ).get_candidates( );
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

		if ( !settings::g_combat.m_autos.revolver.value )
		{
			this->m_revolver_cock_ticks = 0;
			return;
		}

		const auto tick_base = memory::read<std::int32_t>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		constexpr auto interval_per_tick = 0.015625f;
		const auto time = interval_per_tick * static_cast< float >( tick_base + 1 );

		const auto weapon_services = memory::read<std::uintptr_t>( local.pawn + SCHEMA( "C_CSPlayerPawn", "m_pWeaponServices"_hash ) );
		if ( !weapon_services )
		{
			this->m_revolver_cock_ticks = 0;
			return;
		}

		const auto active_weapon_handle = memory::read<std::uint32_t>( weapon_services + SCHEMA( "C_CSPlayer_WeaponServices", "m_hActiveWeapon"_hash ) );
		const auto weapon = systems::g_entities.lookup( active_weapon_handle );
		if ( !weapon )
		{
			this->m_revolver_cock_ticks = 0;
			return;
		}

		const auto next_primary  = memory::read<std::int32_t>( weapon + SCHEMA( "C_BasePlayerWeapon", "m_nNextPrimaryAttackTick"_hash ) );
		const auto postpone      = memory::read<std::int32_t>( weapon + SCHEMA( "C_CSWeaponBase", "m_nPostponeFireReadyTicks"_hash ) );

		// Hammer fully cocked: primary shot is available.
		const auto primary_ready = ( time >= interval_per_tick * static_cast< float >( next_primary ) )
			&& ( time > interval_per_tick * static_cast< float >( postpone ) );

		if ( primary_ready )
		{
			this->m_revolver_cock_ticks = 0;

			// Do not mix fan-fire with the primary release. It uses a different
			// spread seed and invalidates the correction generated by fire_gun().
			this->run_gun( cmd, ctx, local, true );
			return;
		}

		// Cock phase: hold primary attack to prime the hammer.
		// Always cock regardless of global attack cooldown - the server will
		// reject the shot if cooldown isn't met, but cocking must continue.
		cmd->buttons.value        |= cstypes::command_buttons::in_attack;
		cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
		cmd->buttons.value_scroll  |= cstypes::command_buttons::in_attack;

		const auto history_index = cmd->csgo_user_cmd.input_history_size( ) - 1;
		if ( history_index >= 0 )
		{
			cmd->csgo_user_cmd.set_attack1_start_history_index( history_index );
		}

		++this->m_revolver_cock_ticks;
		this->run_gun( cmd, ctx, local, false );
	}

	void rage::apply_autoscope( systems::input::usercmd* cmd, const systems::local::snapshot& local ) const
	{
		if ( !settings::g_combat.m_autos.scope.value )
		{
			return;
		}

		const auto& shared_ctx = g_shared.ctx( );
		if ( shared_ctx.weapon_type != cstypes::weapon_type::sniper )
		{
			return;
		}

		// Already scoped or scoped via automatic scope (zoom level >= 1).
		if ( shared_ctx.is_scoped )
		{
			return;
		}

		// Only autoscope when the ragebot has a valid target to shoot.
		if ( !this->m_firing_this_tick && !this->m_should_stop )
		{
			return;
		}

		// Send secondary attack to scope in (IN_ATTACK2 / IN_ZOOM).
		cmd->buttons.value        |= cstypes::command_buttons::in_second_attack;
		cmd->buttons.value_changed |= cstypes::command_buttons::in_second_attack;
		cmd->buttons.value_scroll  |= cstypes::command_buttons::in_second_attack;
	}

	std::vector<rage::scan_hit> rage::scan_players( const math::vector3& eye, float inaccuracy, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local ) const
	{
		std::vector<std::vector<scan_hit>> per_candidate( candidates.size( ) );

		threadpool::parallel_for( 0, static_cast< int >( candidates.size( ) ), [ & ]( int begin, int end )
			{
				for ( auto ci = begin; ci < end; ++ci )
				{
					auto& cand = candidates[ ci ];
					auto& candidate_hits = per_candidate[ ci ];
					candidate_hits.reserve( 24 );

					for ( auto ri = 0; ri < cand.record_count; ++ri )
					{
						if ( !cand.records[ ri ] || !cand.records[ ri ]->valid )
						{
							continue;
						}

					auto hits = this->scan_player( eye, inaccuracy, ctx, cand, cand.records[ ri ], local );
					const auto has_kill_direct = std::any_of( hits.begin( ), hits.end( ), [ ]( const scan_hit& hit )
						{
							return !hit.penetrated && hit.damage >= static_cast< float >( hit.health );
						} );

					for ( auto& h : hits )
					{
						candidate_hits.push_back( std::move( h ) );
					}

					// A guaranteed kill on a newer pose is both the most reliable
					// outcome and the cheapest to evaluate, so stop early there.
					// Otherwise keep scanning historical poses — a direct hit that
					// cannot kill is not worth settling for while an older pose
					// might expose a lethal (or penetrated-lethal) shot.
					if ( has_kill_direct )
					{
						break;
					}
					}
				}
			}, 0 );

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

	std::vector<rage::scan_hit> rage::scan_player( const math::vector3& eye, float inaccuracy, const aim_context& ctx, candidate& cand, shared::lagcomp::record* record, const systems::local::snapshot& local ) const
	{
		// idk how this happens
		if (!cand.pawn || cand.record_count <= 0 || cand.health <= 0)
			return {};

		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );

		const auto game_scene_node = memory::read<std::uintptr_t>( cand.pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
		const auto hitbox_set = systems::g_hitboxes.query( game_scene_node );
		const auto skeleton = g_shared.lc( ).get_skeleton( *record );
		const auto pen_ctx = g_shared.pen( ).prepare_target( cand.pawn, record );

		const auto force_body = config.body_aim.value;

		std::array<int, 19> scan_order{};
		auto scan_count{ 0 };

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

		if ( config.hitboxes.values[ 3 ] )
		{
			for ( auto idx : { 13, 14, 15, 16, 17, 18 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}

		if ( config.hitboxes.values[ 4 ] )
		{
			for ( auto idx : { 7, 8, 9, 10 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}

		if ( config.hitboxes.values[ 5 ] )
		{
			for ( auto idx : { 11, 12 } )
			{
				scan_order[ scan_count++ ] = idx;
			}
		}

		// Old configs can deserialize with every hitbox disabled. Keep the
		// ragebot operational with the core head and torso hitboxes.
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
			// Speculative point produced by the resolver (fake-pitch / fake-yaw
			// head guesses). These are *guesses* about where the real head is,
			// not measured hitboxes, so selection must treat them as additive:
			// they may upgrade a shot, never block a genuine one.
			bool is_resolver_guess{ false };
		};

		std::vector<trace_point> points;
		points.reserve( static_cast< std::size_t >( scan_count ) * 12 );

		// Cap the number of autowall traces per record. Points are added in
		// hitbox priority order (head/chest first), so trimming the tail only
		// drops low-value multipoints on arms/legs - a large FPS win when the
		// ragebot is engaged close-up.
		constexpr auto k_max_trace_points{ 48 };

		// When very close to the target, reduce multipoint count - at close
		// range all hitboxes are large on screen and a single center point
		// is almost always lethal, so multipoints waste traces for no gain.
		const auto dist_to_target = ( record->origin - eye ).length( );
		const auto close_range = dist_to_target < 300.0f;
		const auto effective_max_points = close_range ? 24 : k_max_trace_points;
		const auto skip_multipoints = close_range && config.pointscale > 0.0f;

		for ( auto idx = 0; idx < scan_count; ++idx )
		{
			if ( static_cast< int >( points.size( ) ) >= effective_max_points )
			{
				break;
			}

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

			if ( config.visualize_aimbot.value )
			{
				std::lock_guard lock( m_debug_mtx );
				m_debug_points.push_back( { center, hitbox_index, true } );
			}

			if ( config.pointscale > 0.0f && !skip_multipoints )
			{
				const auto mps = this->generate_multipoints( *hb, center, bone.rotation, config.pointscale, eye, inaccuracy );

				for ( const auto& mp : mps )
				{
					if ( static_cast< int >( points.size( ) ) >= effective_max_points )
					{
						break;
					}

					const auto duplicate = std::any_of( points.begin( ), points.end( ), [ & ]( const trace_point& point )
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

					if ( config.visualize_aimbot.value )
					{
						std::lock_guard lock( m_debug_mtx );
						m_debug_points.push_back( { mp, hitbox_index, false } );
					}
				}
			}
		}

		// Resolver: when the latest record shows a pitch exploit the displayed
		// head pose is fake. Add candidate head poses at several pitch offsets
		// so the scan can resolve the real head position and shoot it.
		if ( config.resolver_enabled.value && record->fake_pitch_detected && !force_body )
		{
			const systems::hitboxes::entry* head_hb{ nullptr };
			for ( const auto& entry : hitbox_set )
			{
				if ( entry.index == 0 )
				{
					head_hb = &entry;
					break;
				}
			}

			if ( head_hb && head_hb->bone >= 0 && head_hb->bone < 28 )
			{
				const auto& bone = skeleton[ head_hb->bone ];
				if ( bone.position.length_sqr( ) >= 1.0f )
				{
					const auto base_center = bone.rotation.rotate_vector( ( head_hb->mins + head_hb->maxs ) * 0.5f ) + bone.position;
					const auto origin = record->origin;
					const auto offset = base_center - origin;

					for ( const auto pitch : { -89.0f, -60.0f, -30.0f, 30.0f, 60.0f, 89.0f } )
					{
						if ( std::fabsf( pitch ) < 15.0f )
						{
							continue; // near-default pitch is covered by the normal center
						}

						const auto rad = math::helpers::deg_to_rad( pitch );
						const auto c = std::cosf( rad );
						const auto s = std::sinf( rad );
						const auto resolved = origin + math::vector3{ offset.x, offset.y * c - offset.z * s, offset.y * s + offset.z * c };

						const auto duplicate = std::any_of( points.begin( ), points.end( ), [ & ]( const trace_point& point )
							{
								return ( point.position - resolved ).length_sqr( ) < 0.5f;
							} );
						if ( duplicate )
						{
							continue;
						}

						trace_point tp{};
						tp.position = resolved;
						tp.hitbox_index = 0;
						tp.bone_index = head_hb->bone;
						tp.hitbox = *head_hb;
						tp.is_center = false;
						tp.is_resolver_guess = true;
						points.push_back( tp );
					}
				}
			}
		}

		// Resolver: reconstruct the real head pose. The animation system keeps a
		// smoothed aim yaw (CCSPlayerAnimationState::m_flPreviousAimYaw, captured on
		// the record). The difference between that and the recorded mesh rotation is
		// how far the rendered body is wound up / desynced. When it is available and
		// non-trivial, inject the head at that yaw (plus a small margin) instead of
		// guessing. Otherwise fall back to comprehensive brute-force for backwards poses.
		if ( config.resolver_enabled.value && !force_body )
		{
			const auto angle_to_target = math::helpers::calculate_angle( eye, record->origin );
			const auto yaw_diff = std::fabsf( math::helpers::normalize_yaw( record->rotation.y - angle_to_target.y ) );

			const auto anim_delta = record->animstate_valid
				? math::helpers::normalize_yaw( record->prev_aim_yaw - record->rotation.y )
				: 0.0f;

			std::array<float, 8> target_yaws{};
			auto candidate_count{ 0 };

			if ( record->animstate_valid && std::fabsf( anim_delta ) > 25.0f )
			{
				// Normal resolver case - animstate gives us the real yaw
				const auto real_yaw = math::helpers::normalize_yaw( record->rotation.y + anim_delta );
				target_yaws[ candidate_count++ ] = real_yaw;
				target_yaws[ candidate_count++ ] = math::helpers::normalize_yaw( real_yaw - 8.0f );
				target_yaws[ candidate_count++ ] = math::helpers::normalize_yaw( real_yaw + 8.0f );
				target_yaws[ candidate_count++ ] = math::helpers::normalize_yaw( real_yaw - 15.0f );
				target_yaws[ candidate_count++ ] = math::helpers::normalize_yaw( real_yaw + 15.0f );
			}
			else if ( yaw_diff > 100.0f )
			{
				// Backwards anti-aim: head capsule is edge-on.
				// Try multiple angles: straight backwards, 45°, 135°, 225°, 315°, and some jitter angles
				target_yaws[ candidate_count++ ] = 180.0f;
				target_yaws[ candidate_count++ ] = 135.0f;
				target_yaws[ candidate_count++ ] = 225.0f;
				target_yaws[ candidate_count++ ] = 45.0f;
				target_yaws[ candidate_count++ ] = 315.0f;
				target_yaws[ candidate_count++ ] = 90.0f;
				target_yaws[ candidate_count++ ] = 270.0f;
				target_yaws[ candidate_count++ ] = 0.0f;

				// Vendetta-style multipoint_head_minimal: add capsule edge points at ~33 degrees
				// These target the actual capsule geometry edges for backwards AA
				const systems::hitboxes::entry* backwards_head_hb{ nullptr };
				for ( const auto& entry : hitbox_set )
				{
					if ( entry.index == 0 )
					{
						backwards_head_hb = &entry;
						break;
					}
				}

				if ( backwards_head_hb && backwards_head_hb->bone >= 0 && backwards_head_hb->bone < 28 )
				{
					const auto& bone = skeleton[ backwards_head_hb->bone ];
					if ( bone.position.length_sqr( ) >= 1.0f )
					{
						const auto head_center = bone.rotation.rotate_vector( ( backwards_head_hb->mins + backwards_head_hb->maxs ) * 0.5f ) + bone.position;
						const auto origin = record->origin;
						const auto offset = head_center - origin;

						constexpr float angle_33 = 0.575958653f; // ~33 degrees
						const auto cos_33 = std::cosf( angle_33 );
						const auto sin_33 = std::sinf( angle_33 );
						const auto diagonal = math::vector3{ cos_33, sin_33, 0.0f } * backwards_head_hb->radius;

						// Add diagonal edge points for both capsule ends
						trace_point tp1{};
						tp1.position = head_center + diagonal;
						tp1.hitbox_index = 0;
						tp1.bone_index = backwards_head_hb->bone;
						tp1.hitbox = *backwards_head_hb;
						tp1.is_center = false;
						points.push_back( tp1 );

						trace_point tp2{};
						tp2.position = head_center - diagonal;
						tp2.hitbox_index = 0;
						tp2.bone_index = backwards_head_hb->bone;
						tp2.hitbox = *backwards_head_hb;
						tp2.is_center = false;
						points.push_back( tp2 );

						// Also add the opposite diagonal
						const auto diagonal2 = math::vector3{ cos_33, -sin_33, 0.0f } * backwards_head_hb->radius;
						trace_point tp3{};
						tp3.position = head_center + diagonal2;
						tp3.hitbox_index = 0;
						tp3.bone_index = backwards_head_hb->bone;
						tp3.hitbox = *backwards_head_hb;
						tp3.is_center = false;
						points.push_back( tp3 );

						trace_point tp4{};
						tp4.position = head_center - diagonal2;
						tp4.hitbox_index = 0;
						tp4.bone_index = backwards_head_hb->bone;
						tp4.hitbox = *backwards_head_hb;
						tp4.is_center = false;
						points.push_back( tp4 );
					}
				}
			}
			else if ( record->animstate_valid && std::fabsf( anim_delta ) > 10.0f )
			{
				// Edge case: some desync but not enough for full resolver
				const auto real_yaw = math::helpers::normalize_yaw( record->rotation.y + anim_delta );
				target_yaws[ candidate_count++ ] = real_yaw;
				target_yaws[ candidate_count++ ] = math::helpers::normalize_yaw( real_yaw - 10.0f );
				target_yaws[ candidate_count++ ] = math::helpers::normalize_yaw( real_yaw + 10.0f );
			}

			if ( candidate_count > 0 )
			{
				const systems::hitboxes::entry* head_hb{ nullptr };
				for ( const auto& entry : hitbox_set )
				{
					if ( entry.index == 0 )
					{
						head_hb = &entry;
						break;
					}
				}

				if ( head_hb && head_hb->bone >= 0 && head_hb->bone < 28 )
				{
					const auto& bone = skeleton[ head_hb->bone ];
					if ( bone.position.length_sqr( ) >= 1.0f )
					{
						const auto head_center = bone.rotation.rotate_vector( ( head_hb->mins + head_hb->maxs ) * 0.5f ) + bone.position;
						const auto origin = record->origin;
						// XY offset from player foot origin to head center.
						const auto ox = head_center.x - origin.x;
						const auto oy = head_center.y - origin.y;
						const auto oz = head_center.z - origin.z;

						for ( auto ci = 0; ci < candidate_count; ++ci )
						{
							const auto rad = math::helpers::deg_to_rad( target_yaws[ ci ] );
							const auto c = std::cosf( rad );
							const auto s = std::sinf( rad );
							// Rotate XY offset by the target yaw, keep Z.
							const auto rx = ox * c - oy * s;
							const auto ry = ox * s + oy * c;
							const auto resolved = math::vector3{ origin.x + rx, origin.y + ry, origin.z + oz };

							const auto duplicate = std::any_of( points.begin( ), points.end( ), [ &resolved ]( const trace_point& point )
								{
									return ( point.position - resolved ).length_sqr( ) < 1.0f;
								} );
							if ( duplicate )
							{
								continue;
							}

							trace_point tp{};
							tp.position = resolved;
							tp.hitbox_index = 0;
							tp.bone_index = head_hb->bone;
							tp.hitbox = *head_hb;
							tp.is_center = false;
							tp.is_resolver_guess = true;
							points.push_back( tp );
						}
					}
				}
			}
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

			shared::penetration::result pen{};
			if ( !g_shared.pen( ).run( eye, tp.position, pen_ctx, local.pawn, local.team, pen ) )
			{
				continue;
			}

			if ( pen.damage < cand.min_damage )
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
				center_sufficient[ tp.hitbox_index ] = !pen.penetrated || pen.damage >= static_cast< float >( cand.health );
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
			h.is_resolver_guess = tp.is_resolver_guess;
			h.penetrated = pen.penetrated;
			h.meets_min_damage = pen.damage >= cand.min_damage;
			h.pawn = cand.pawn;
			h.health = cand.health;
			h.priority = cand.priority;
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
				// Full-damage hits are strictly better than fallback ones, but keep
				// fallbacks in the top-k so the expensive hitchance stage can still
				// pick a body when the head is un-shootable.
				const auto min_dmg_bonus = h.meets_min_damage ? 8000.0f : 0.0f;

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
			bool is_backwards;
			int group_idx;
		};

		std::vector<evaluated_hit> evaluated;
		evaluated.reserve( hits.size( ) );

		const auto& config = settings::g_combat.m_ragebot.get_group( g_shared.ctx( ).weapon_type );
		auto needed_hc = config.hitchance_override.value
			? static_cast< float >( config.hitchance_override_value ) / 100.0f
			: static_cast< float >( config.hitchance ) / 100.0f;

		// Adaptive hitchance: every miss streak relaxes the requirement a step,
		// so the bot keeps taking fights instead of stalling on an unshootable shot.
		if ( config.adaptive_hitchance.value )
		{
			needed_hc = std::max( 0.0f, needed_hc - this->m_adaptive_hc_boost );
		}

		for ( auto& group : groups )
		{
			if ( !group.record || !group.record->valid )
			{
				continue;
			}

			// Backwards anti-aim: the recorded pose faces away from us, so the
			// head capsule is edge-on to the bullet line and unreliable to hit.
			// Compute the flag once per record so we can bodytap those targets.
			const auto rec = group.record;
			auto eye = g_shared.get_shoot_position( );
			for ( const auto gi : group.hit_indices )
			{
				if ( hits[ gi ].source_eye.position.length_sqr( ) > 0.1f )
				{
					eye = hits[ gi ].source_eye.position;
					break;
				}
			}
			const auto angle_to_target = math::helpers::calculate_angle( eye, rec->origin );
			const auto is_backwards = std::fabsf( math::helpers::normalize_yaw( rec->rotation.y - angle_to_target.y ) ) > 100.0f;
			const auto group_idx = static_cast< int >( &group - groups.data( ) );

			for ( const auto idx : group.hit_indices )
			{
				const auto& h = hits[ idx ];

				if ( !h.record || !h.record->valid )
				{
					continue;
				}

				if ( h.bone_index < 0 || h.bone_index >= 28 )
				{
					continue;
				}

				const auto& bone = group.record->bones[ h.bone_index ];
				const auto hc = config.no_spread.value
					? 1.0f
					: g_shared.calculate_hitchance( h.source_eye.position, h.aim_angle, h.hitbox, bone, eval_inaccuracy, aim_ctx.spread );
				const auto hp = static_cast< float >( h.health );
				const auto can_kill = h.damage >= hp;
				const auto is_head = h.hitbox_index == 0;
				const auto passes_hitchance = config.no_spread.value || hc >= needed_hc;
				// A hit that cannot pass hitchance will never actually fire, so it must never
				// beat a hit that can. Otherwise the bot commits to an unshootable head
				// (e.g. capsule oriented along the line of sight for backwards / jitter AA)
				// and refuses to take a viable body shot on the same enemy.
				auto score = passes_hitchance ? 10000000.0f : 0.0f;

				// Resolver guesses are speculative: they guess where the real head
				// is when the server pose lies. A guess that only *marginally*
				// clears the gate wins the +10M bucket here, then gets rejected by
				// fire_gun's re-validation against the inaccuracy at the moment of
				// the shot - so the bot commits to a shot it then refuses to take
				// and simply stops shooting. Demote any guess that does not
				// comfortably clear the gate so a genuine measured hitbox always
				// wins. The resolver stays purely additive: it can upgrade a shot,
				// never block one.
				if ( h.is_resolver_guess )
				{
					if ( !passes_hitchance )
					{
						score -= 20000000.0f;
					}
					else if ( hc < needed_hc + 0.15f )
					{
						score -= 4000000.0f;
					}
				}

				// Below-min-damage hits are only kept as fallbacks. Penalise them so a
				// full-damage passing shot always wins, but keep them ahead of a
				// non-passing head that the bot cannot actually fire.
				if ( !h.meets_min_damage )
				{
					score -= 5000000.0f;
				}

				if ( can_kill )
				{
					score += 100000.0f + hc * 10000.0f;
					// Head shots that kill are the most valuable outcome — strongly prefer them
					// over body kills so the bot favours clean headshots when available.
					if ( is_head )
					{
						score += 500000.0f;
					}
				}
				else
				{
					score += h.damage * hc * 100.0f + h.damage * 5.0f;
					// Even when non-lethal, prefer head if it deals meaningful damage so the
					// bot doesn't dump body shots when the head is exposed and viable.
					if ( is_head )
					{
						score += 20000.0f;
					}
				}

				score += h.penetrated ? 0.0f : 250.0f;
				score += h.is_center ? 50.0f : 0.0f;
				score += static_cast< float >( hitgroup_priority( h.hitbox_index ) ) * 500.0f;
				score -= h.fov * 0.1f;

				// Target priority: favour enemies that threaten us or are close
				// to death so the bot clears pressure targets first.
				if ( config.target_priority.value )
				{
					score += h.priority * 500.0f;
				}

				// Baim if the head hitchance is low: a low-probability headshot is
				// worse than a reliable body hit. Only devalue a head that cannot
				// kill - a headshot that would win the fight must not lose to a
				// bodytap purely because its capsule is statistically unlikely to
				// be hit (e.g. backwards anti-aim orients the head edge-on).
				if ( config.baim_if_head_low_hc.value && is_head && hc < config.baim_head_hc_threshold.value )
				{
					if ( !can_kill )
					{
						score -= 5000000.0f;
					}
				}

				// A direct (non-penetrated) kill is strictly more reliable than
				// killing through geometry — wallbang damage estimates carry
				// surface-variance error. Reward it so the bot never settles for
				// a risky wallbang when a clean shot kills.
				if ( can_kill && !h.penetrated )
				{
					score += 20000.0f;
				}

				evaluated.push_back( evaluated_hit{ idx, hc, score, is_backwards, group_idx } );
			}
		}

		// If the head cannot be taken reliably right now (backwards anti-aim,
		// edge-on jitter, or a near-miss head capsule), never stall the fight:
		// fall back to the highest-damage hit that the scan actually found —
		// a lethal headshot when one exists, otherwise a body shot that meets
		// min damage. This mirrors the reference scan which commits damage-first.
		if ( !groups.empty( ) )
		{
			std::vector<bool> head_reliable( groups.size( ), false );
			for ( const auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				if ( h.hitbox_index == 0 && e.hitchance >= needed_hc && h.damage >= static_cast< float >( h.health ) )
				{
					head_reliable[ e.group_idx ] = true;
				}
			}

			// Per-record best body damage: used to promote the strongest body
			// shot on a target whose head cannot be taken this tick.
			std::vector<float> best_body_damage( groups.size( ), 0.0f );
			for ( const auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				if ( h.hitbox_index != 0 && h.meets_min_damage )
				{
					best_body_damage[ e.group_idx ] = std::max( best_body_damage[ e.group_idx ], h.damage );
				}
			}

			for ( auto& e : evaluated )
			{
				const auto& h = hits[ e.hit_index ];
				const auto is_head = h.hitbox_index == 0;

				// If the group already offers a shootable head, leave its scores
				// untouched — a clean headshot always wins here.
				if ( head_reliable[ e.group_idx ] )
				{
					continue;
				}

				if ( is_head )
				{
					// A head that cannot pass the hitchance gate (edge-on
					// backwards AA) will not land, so it must never beat the
					// highest-damage body of the same target — even if it
					// could theoretically kill. Otherwise the bot keeps
					// committing to an un-hittable head and never bodyshots.
					if ( e.hitchance < needed_hc )
					{
						e.score -= 5000000.0f;
					}
				}
				else if ( h.meets_min_damage && h.damage >= best_body_damage[ e.group_idx ] )
				{
					// Promote the strongest body panel so we actually shoot instead
					// of stalling when the head can't be taken right now. Only the
					// highest-damage body gets the bonus; weaker panes stay below.
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

	void rage::fire_gun( systems::input::usercmd* cmd, target& tgt, bool was_forced, const math::vector3& shoot_eye, const systems::local::snapshot& local, bool subtick_attack )
	{
		if ( !tgt.hit.record || !tgt.hit.record->valid )
		{
			return;
		}

		this->m_firing_this_tick = true;

		const auto base = cmd->csgo_user_cmd.mutable_base( );
		const auto tick_base = memory::read<int>( local.controller + SCHEMA( "CBasePlayerController", "m_nTickBase"_hash ) );
		this->m_last_shot_tick = static_cast< std::uint32_t >( tick_base );
		const auto& shared_ctx = g_shared.ctx( );
		const auto& config = settings::g_combat.m_ragebot.get_group( shared_ctx.weapon_type );
		const auto aim_punch = g_shared.get_aim_punch( local.pawn );
		auto aim_angle = config.no_spread.value ? math::helpers::calculate_angle( shoot_eye, tgt.hit.position ) : tgt.hit.aim_angle;

		if ( !config.no_spread.value )
		{
			// Re-validate hitchance with the accuracy AT the moment the shot is
			// about to fire. Between target selection and this command the player
			// may still be decelerating, jumping or holding, so the selection-time
			// inaccuracy can be far lower than what the server will apply. Firing
			// with a stale hit chance is exactly how "72% hitchance" ends up
			// spraying wide: refuse instead.
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

				tgt.hitchance = fired_hc;
			}
		}

		if ( config.no_spread.value )
		{
			auto stamp_tick = tick_base;
			auto stamp_frac{ 0.0f };

			if ( !tgt.hit.source_eye.is_uninterpolated )
			{
				auto tick_add = [ ]( int t, float f, int dt, float df )
					{
						f += df;
						auto carry = static_cast< int >( std::floor( f ) );
						f -= static_cast< float >( carry );
						return std::pair{ t + dt + carry, f };
					};

				auto shift{ 0 };
				if ( subtick_attack )
				{
					// Doubletap bursts alternate their player tick by one so the
					// shots of a burst resolve across consecutive ticks.
					shift = this->m_dt_shot_count & 1;
				}

				std::tie( stamp_tick, stamp_frac ) = tick_add( tgt.hit.source_eye.player_tick, tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int, tgt.hit.source_eye.lerp_ticks_frac );
				stamp_tick += shift;
			}

			const auto corrected = g_shared.find_spread_correction( aim_angle, stamp_tick, g_shared.get_inaccuracy( false ), g_shared.get_spread( ) );
			if ( corrected.x == 0.0f && corrected.y == 0.0f && corrected.z == 0.0f )
			{
				this->m_firing_this_tick = false;
				return;
			}

			aim_angle = corrected;
		}

		g_shared.last_shoot_tick( ) = tick_base;

		// Record the shot for adaptive hitchance / miss tracking. The target
		// health is snapshotted so a later read can tell a hit from a miss.
		this->m_last_shot_tick_when = tick_base;
		this->m_last_shot_pawn = tgt.hit.pawn;
		this->m_last_shot_health = tgt.hit.health;

		if ( settings::g_misc.m_impacts.console_log.value )
		{
			const auto hitgroup_name = systems::g_hitboxes.hitgroup_to_name( tgt.hit.hitgroup );
			const auto bt_delta = g_shared.ctx( ).current_tick - tgt.hit.record->tick;
			logging::console::print(
				xs( "[rage] shot target hp {} for {:.0f} in {} (hc {:.0f}%, bt {}t{})" ),
				tgt.hit.health,
				tgt.hit.damage,
				hitgroup_name,
				tgt.hitchance * 100.0f,
				bt_delta,
				was_forced ? xs( ", forced" ) : ""
			);
		}

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

			entry->set_render_tick_count( record_time.tick + 2 );
			entry->set_render_tick_fraction( 0.0f );

			if ( !tgt.hit.source_eye.is_uninterpolated )
			{
				auto tick_add = [ ]( int t, float f, int dt, float df )
					{
						f += df;
						auto carry = static_cast< int >( std::floor( f ) );
						f -= static_cast< float >( carry );
						return std::pair{ t + dt + carry, f };
					};

				auto shift{ 0 };
				if ( subtick_attack )
				{
					// Doubletap bursts alternate their player tick by one so the
					// shots of a burst resolve across consecutive ticks.
					shift = this->m_dt_shot_count & 1;
				}

				const auto [stamp_tick, stamp_frac] = tick_add( tgt.hit.source_eye.player_tick, tgt.hit.source_eye.player_frac, tgt.hit.source_eye.lerp_ticks_int, tgt.hit.source_eye.lerp_ticks_frac );

				entry->set_player_tick_count( stamp_tick + shift );
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

		if ( !subtick_attack )
		{
			// R8 spread correction is generated for its primary shot.  Fan-fire
			// (IN_ATTACK2) has different timing/seed behaviour, which made every
			// auto-revolver no-spread shot miss.  Keep the normal cock-and-release
			// path and always stamp the primary attack history.
			cmd->buttons.value |= cstypes::command_buttons::in_attack;
			cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
			cmd->buttons.value_scroll |= cstypes::command_buttons::in_attack;

			if ( history_size > 0 )
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
		if ( !subtick_attack && facing_away && settings::g_combat.m_antiaim.hide_shots.value )
		{
			command_aim.x = 179.9f;
			command_aim.y = std::remainderf( punched_aim.y + 180.0f, 360.0f );
		}

		if ( const auto angles = base->mutable_viewangles( ) )
		{
			angles->set_x( command_aim.x );
			angles->set_y( command_aim.y );
		}

		// The command angles just changed basis under antiaim's feet. Movement
		// values are still expressed in the fake basis, so the engine would read
		// WASD against the aim direction and the path snaps on every shot. Re-base
		// them onto the angle we actually installed. No-op without antiaim.
		features::combat::g_misc.antiaim( ).rebasis_movement( cmd, command_aim );

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
		this->m_last_shot_tick = static_cast< std::uint32_t >( tick_base );

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

			entry->set_render_tick_count( record_time.tick + 2 );
			entry->set_render_tick_fraction( 0.0f );

			if ( !tgt.hit.source_eye.is_uninterpolated )
			{
				auto tick_add = [ ]( int t, float f, int dt, float df )
					{
						f += df;
						auto carry = static_cast< int >( std::floor( f ) );
						f -= static_cast< float >( carry );
						return std::pair{ t + dt + carry, f };
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

		// Keep points inside the part of the hitbox reachable by the full spread cone.
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

		// Build a view-relative frame so points rotate correctly above and below us.
		const auto shoot_dir = ( center - shoot_pos ).normalized( );
		const auto ang       = math::helpers::vector_to_angle( shoot_dir );

		math::vector3 left{}, up{};
		math::helpers::angle_vectors_left( ang, nullptr, &left, &up );

		// angle_vectors_left returns left, so negate it for right.
		const auto right = math::vector3{ -left.x, -left.y, -left.z };

		// Trace from outside through the center to find the real capsule surface.
		// This is accurate around rounded end caps, where radius offsets are not.
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
				// Intersect box hitboxes in bone space using their directional support.
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

		// Vendetta-style rotating multipoints for head hitbox (index 0)
		// Uses m_head_angle which rotates each tick for better coverage against AA
		if ( hitbox.index == 0 && hitbox.radius > 0.001f )
		{
			constexpr float angle_33 = 0.575958653f; // ~33 degrees - targets capsule edges like vendetta's multipoint_head_minimal
			const auto cos_val = std::cos( this->m_head_angle );
			const auto sin_val = std::sin( this->m_head_angle );
			const auto angle_offset = ( right * cos_val + up * sin_val ) * hitbox.radius;

			// Add the rotating edge points (like vendetta's multipoint_head_minimal)
			out.push_back( capsule_a + angle_offset );
			out.push_back( capsule_a - angle_offset );
			out.push_back( capsule_b + angle_offset );
			out.push_back( capsule_b - angle_offset );
		}

		switch ( hitbox.index )
		{
		case 0: // head
		{
			out.reserve( 4 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			out.push_back( scaled_surface( up ) );
			out.push_back( scaled_surface( -up ) );
			break;
		}

		case 2: case 3: // stomach / pelvis
		{
			out.reserve( 2 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			break;
		}

		case 4: case 5: case 6: // chest
		{
			out.reserve( 3 );
			out.push_back( scaled_surface( right ) );
			out.push_back( scaled_surface( -right ) );
			if ( hitbox.index == 6 )
			{
				out.push_back( scaled_surface( up ) );
			}
			break;
		}

		case 7: case 8: case 9: case 10: case 11: case 12: // legs / feet
		{
			out.reserve( 2 );
			out.push_back( capsule_a );
			out.push_back( capsule_b );
			break;
		}

		case 13: case 14: case 15: case 16: case 17: case 18: // arms
		{
			out.reserve( 1 );
			out.push_back( capsule_b );
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
		// Stop for a short window after each shot so the player settles between consecutive shots.
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

		// In-air autostop: for all weapons, stop if we'll become shootable during descent
		// This allows hitting shots at the top of a jump/apex when accuracy recovers
		if ( velocity.z > 140.0f )
		{
			// Still ascending rapidly - accuracy will only get worse
			return false;
		}

		const auto sv_gravity = CONVAR( "sv_gravity" )->get<float>( );
		const auto sv_friction = CONVAR( "sv_friction" )->get<float>( );
		const auto sv_stopspeed = CONVAR( "sv_stopspeed" )->get<float>( );

		const auto inac_jump_initial = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpInitial"_hash ) );
		const auto inac_jump_apex = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flInaccuracyJumpApex"_hash ) );
		const auto shootable_threshold = inac_jump_apex + 0.001f;
		const auto early_threshold = inac_jump_initial * 0.55f + inac_jump_apex * 0.45f;
		const auto air_inaccuracy = g_shared.get_air_inaccuracy( velocity.z, inac_jump_initial, inac_jump_apex );

		// Already shootable now
		if ( air_inaccuracy <= shootable_threshold || air_inaccuracy <= early_threshold )
		{
			return true;
		}

		// Simulate descent to see when we'll become shootable
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
			// Won't become shootable in reasonable time
			return false;
		}

		// Check if we can stop horizontally before becoming shootable
		const auto speed_2d = velocity.length_2d( );
		const auto max_speed = memory::read<float>( shared_ctx.weapon_vdata + SCHEMA( "CCSWeaponBaseVData", "m_flMaxSpeed"_hash ) );
		const auto accurate_threshold = max_speed * 0.34f;

		if ( speed_2d <= accurate_threshold )
		{
			// Already slow enough
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

		// Stop if we can become accurate before or shortly after becoming shootable
		// Allow +2 tick buffer for the shot to be sent
		return ticks_to_shootable <= ticks_to_stop + 2;
	}

	float rage::get_min_damage( const settings::combat::ragebot::weapon_group& config, int target_health, bool override_active ) const
	{
		if ( override_active )
		{
			return static_cast< float >( config.min_damage_override_value );
		}

		const auto hp = static_cast< float >( target_health );

		if ( config.min_damage_hp_plus_one.value )
		{
			return hp + 1.0f;
		}

		const auto base = static_cast< float >( config.min_damage );

		if ( config.adaptive_min_damage.value )
		{
			// Low-health targets: require the kill. Healthy targets: never
			// demand more damage than the target has and relax the base so the
			// bot keeps trading damage instead of waiting for a perfect shot.
			if ( hp <= 45.0f )
			{
				return hp + 1.0f;
			}

			return std::min( base, std::max( base * 0.5f, hp * 0.55f ) );
		}

		if ( hp < base )
		{
			return hp + 1.0f;
		}

		return base;
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

		// get_shoot_position() can be zero before prediction has populated the
		// weapon-services shoot history. The crosshair needs the current eye now.
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
		const auto cx = std::floorf( static_cast< float >( screen_w ) * 0.5f );
		const auto cy = std::floorf( static_cast< float >( screen_h ) * 0.5f );
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

} // namespace features::combat
