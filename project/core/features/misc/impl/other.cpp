#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>
namespace features::misc {
	namespace {

		[[nodiscard]] std::string controller_name( std::uintptr_t controller )
		{
			if ( !controller )
			{
				return {};
			}

			const auto name_ptr = memory::safe_read<std::uintptr_t>(
				controller + SCHEMA( "CCSPlayerController", "m_sSanitizedPlayerName"_hash ) ).value_or( 0 );
			return memory::read_string( name_ptr, 127 );
		}

		void submit_name_change( const std::string& display_name )
		{
			if ( display_name.empty( ) )
			{
				return;
			}

			other::s_display_name = display_name;
			other::s_name_change_pending = true;

			// Build the command with the actual name instead of using "x" placeholder
			std::string cmd = xs( "setinfo name \"" );
			cmd += display_name;
			cmd += xs( "\"" );
			memory::call<void>( PATTERN( patterns::engine_client_cmd ), addresses::globals::source2engine_to_client, 0, cmd.c_str( ), 0x7ffef001 );

			other::s_name_change_pending = false;
		}

	} // namespace

	void other::on_round_start( )
	{
		this->do_autobuy( );
	}

	void other::on_player_death( std::uintptr_t event )
	{
		if ( !event )
		{
			return;
		}

		const auto attacker_key = cstypes::event_hash{ 0, "attacker" };
		const auto attacker = memory::call<std::uintptr_t>( PATTERN (patterns::game_event_get_controller), event, &attacker_key );
	}

	void other::on_frame_stage_notify( )
	{
		this->do_player_alpha_changing( );
		this->do_reveal_radar( );
		this->do_name_changing( );
		this->do_anti_afk( );
		this->do_sniper_crosshair_patch( );
	}

	void other::on_create_move( systems::input::usercmd* cmd )
	{
		this->do_quickswitch( );
		this->do_quick_plant( );
		this->do_no_land_inaccuracy( );
	}

	void other::do_reveal_radar( ) const
	{
		if ( !settings::g_misc.reveal_radar.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) )
		{
			return;
		}

		const auto spotted_state_offset = SCHEMA( "C_CSPlayerPawn", "m_entitySpottedState"_hash );
		const auto spotted_offset = SCHEMA( "EntitySpottedState_t", "m_bSpotted"_hash );

		for ( const auto& player : systems::g_entities.get_by_type( systems::entities::type::player ) )
		{
			const auto controller = player.ptr;
			if ( !controller || !memory::read<bool>( controller + SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash ) ) )
			{
				continue;
			}

			const auto pawn_handle = memory::read<std::uint32_t>( controller + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) );
			const auto pawn = systems::g_entities.lookup( pawn_handle );
			if ( !pawn || pawn == local.view_pawn( ) )
			{
				continue;
			}

			const auto team = memory::read<int>( pawn + SCHEMA( "C_BaseEntity", "m_iTeamNum"_hash ) );
			if ( !local.is_this_other_team( team ) )
			{
				continue;
			}

			memory::write<bool>( pawn + spotted_state_offset + spotted_offset, true );
		}
	}

	void other::do_autobuy( ) const
	{
		if ( !settings::g_misc.m_autobuy.enabled )
		{
			return;
		}

		std::string cmd {};

		switch ( settings::g_misc.m_autobuy.primary_weapon )
		{
		case 1: cmd += xs( "buy ak47; buy m4a1; " ); break;
		case 2: cmd += xs( "buy sg556; buy aug; " ); break;
		case 3: cmd += xs( "buy ssg08; " ); break;
		case 4: cmd += xs( "buy awp; " ); break;
		case 5: cmd += xs( "buy g3sg1; buy scar20; " ); break;
		}

		if ( settings::g_misc.m_autobuy.armor )
		{
			cmd += xs( "buy vesthelm; buy vest; " );
		}

		if ( settings::g_misc.m_autobuy.taser )
		{
			cmd += xs( "buy taser; " );
		}

		if ( settings::g_misc.m_autobuy.defuser )
		{
			cmd += xs( "buy defuser; " );
		}

		switch ( settings::g_misc.m_autobuy.secondary_weapon )
		{
		case 1: cmd += xs( "buy elite; " ); break;
		case 2: cmd += xs( "buy fiveseven; buy tec9; " ); break;
		case 3: cmd += xs( "buy deagle; " ); break;
		case 4: cmd += xs( "buy revolver; " ); break;
		}

		for ( auto i = 0; i < 5; ++i )
		{
			if ( !settings::g_misc.m_autobuy.grenades[ i ] )
			{
				continue;
			}

			switch ( i )
			{
			case 0: cmd += xs( "buy molotov; buy incgrenade; " ); break;
			case 1: cmd += xs( "buy hegrenade; " ); break;
			case 2: cmd += xs( "buy smokegrenade; " ); break;
			case 3: cmd += xs( "buy flashbang; " ); break;
			case 4: cmd += xs( "buy decoy; " ); break;
			}
		}

		if ( !cmd.empty( ) )
		{
			memory::call<void>(PATTERN (patterns::engine_client_cmd), addresses::globals::source2engine_to_client, 0, cmd.c_str( ), 0x7ffef001 );
		}
	}

	void other::do_player_alpha_changing( )
	{
		const auto local = systems::g_local.get( );

		if ( !local.pawn || !local.is_alive )
		{
			if ( this->m_is_alpha_changed && local.pawn )
			{
				memory::call<void>( PATTERN (patterns::game_event_get_string), local.pawn, 255 );
			}

			this->m_is_alpha_changed = false;
			return;
		}

		if ( !settings::g_esp.m_local_alpha.enabled.value )
		{
			if ( this->m_is_alpha_changed )
			{
				this->m_is_alpha_changed = false;
				memory::call<void>( PATTERN (patterns::game_event_get_string), local.pawn, 255 );
			}

			return;
		}

		const auto is_scoped = memory::read<bool>( local.pawn + SCHEMA( "C_CSPlayerPawn", "m_bIsScoped"_hash ) );
		const auto should_apply = !settings::g_esp.m_local_alpha.only_scoped.value || is_scoped;

		if ( should_apply )
		{
			this->m_is_alpha_changed = true;
			const auto alpha = static_cast< std::uint8_t >( settings::g_esp.m_local_alpha.opacity.value * 255.0f );
			memory::call<void>( PATTERN (patterns::game_event_get_string), local.pawn, alpha );
		}
		else
		{
			if ( this->m_is_alpha_changed )
			{
				this->m_is_alpha_changed = false;
				memory::call<void>( PATTERN (patterns::game_event_get_string), local.pawn, 255 );
			}
		}
	}

	void other::do_name_changing( )
	{
		const auto local = systems::g_local.get( );
		const auto& cfg = settings::g_misc.m_name_changer;
		const auto enabled = cfg.clantag.value || cfg.override_name.value;

		if ( !enabled )
		{
			if ( this->m_name_changer_active && local.controller && !this->m_original_name.empty( ) )
			{
				// Restore the original name when disabled
				submit_name_change( this->m_original_name );
			}

			this->m_name_changer_active = false;
			this->m_name_changer_controller = 0;
			this->m_original_name.clear( );
			this->m_last_sent_name.clear( );
			this->m_clantag_index = 0;
			other::s_display_name.clear( );
			return;
		}

		if ( !local.controller )
		{
			return;
		}

		if ( !this->m_name_changer_active )
		{
			this->m_original_name = controller_name( local.controller );
			if ( this->m_original_name.empty( ) )
			{
				this->m_original_name = xs( "Player" );
			}

			this->m_name_changer_active = true;
			this->m_name_changer_controller = local.controller;
			this->m_last_sent_name.clear( );
			this->m_clantag_index = 0;
		}
		else if ( this->m_name_changer_controller != local.controller )
		{
			// Keep the captured real name across map loads, where the controller may be recreated.
			this->m_name_changer_controller = local.controller;
			this->m_last_sent_name.clear( );
		}

		// Get base name (either custom or original)
		const auto& configured_name = cfg.name.value;
		const auto& base_name = cfg.override_name.value && !configured_name.empty( )
			? configured_name
			: this->m_original_name;

		std::string display_name = base_name;

		if ( cfg.clantag.value )
		{
			// Animated clantag: " f", " fl", " flo", " flow", ... " flowsense"
			constexpr std::string_view full_tag{ "Flowsense" };
			// Derived from the tag so the type-on animation can never desync from it.
			constexpr auto animation_frames{ full_tag.size( ) };

			// Read current server time for animation timing
			const auto global_vars = memory::safe_read<std::uintptr_t>( addresses::globals::global_vars ).value_or( 0 );
			const auto current_time = global_vars ? memory::safe_read<float>( global_vars + 0x30 ).value_or( 0.0f ) : 0.0f;

			// Change animation frame every 0.4 seconds
			const auto frame = static_cast< std::size_t >( current_time / 0.4f ) % ( animation_frames * 2 );

			// First half: build the tag up character by character.
			// Second half: hold the full tag.
			const auto char_count = ( frame < animation_frames ) ? frame + 1 : animation_frames;

			// Build the animated tag with leading space and PREPEND to base name
			std::string animated_name;
			animated_name.reserve( char_count + 2 + base_name.length( ) );
			animated_name += ' ';
			animated_name.append( full_tag.substr( 0, char_count ) );
			animated_name += ' ';
			animated_name += base_name;

			display_name = animated_name;
		}

		if ( display_name == this->m_last_sent_name )
		{
			return;
		}

		submit_name_change( display_name );
		this->m_last_sent_name = std::move( display_name );
	}

	void other::do_kill_feed_preservation( )
	{
		const auto local = systems::g_local.get ();

		if (!local.pawn || !local.is_alive) {
			return;
		}

		const auto hud_element = memory::call<std::uintptr_t>(PATTERN (patterns::find_hud_element), xs( "CCSGO_HudDeathNotice" ) );
		if ( !hud_element )
		{
			return;
		}

		memory::write<float> (hud_element + 0x58, settings::g_misc.preserve_killfeed ? 1000.0f : 1.5f);

		float spawntime = memory::read<float> (local.pawn + SCHEMA ("C_CSPlayerPawnBase", "m_flLastSpawnTimeIndex"_hash));
		if ( m_last_spawntime != spawntime )
		{
			const auto clear_death_notices = PATTERN (patterns::hud_death_notice_clear);
			if ( clear_death_notices )
			{
				memory::call<void>( clear_death_notices, hud_element - 0x20 );
			}

			m_last_spawntime = spawntime;
		}
	}

	void other::do_anti_afk( )
	{
		if ( !settings::g_misc.anti_afk.value )
		{
			this->m_next_anti_afk_time = 30.0f;
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || !local.pawn )
		{
			this->m_next_anti_afk_time = 30.0f;
			return;
		}

		// Read the current server time from the global vars (interval_per_tick * tick).
		const auto global_vars = memory::safe_read<std::uintptr_t>( addresses::globals::global_vars ).value_or( 0 );
		if ( !global_vars )
		{
			return;
		}

		const auto current_time = memory::safe_read<float>( global_vars + 0x30 ).value_or( 0.0f );
		if ( current_time >= this->m_next_anti_afk_time )
		{
			// Jump to reset the afk timer. Sending a "+jump" via the engine keeps
			// it out of our usercmd pipeline and avoids fighting the bhop feature.
			memory::call<void>( PATTERN( patterns::engine_client_cmd ), addresses::globals::source2engine_to_client, 0, xs( "+jump; -jump" ), 0x7ffef001 );
			this->m_next_anti_afk_time = current_time + 30.0f;
		}
	}

	void other::do_quickswitch( )
	{
		if ( !settings::g_misc.quickswitch.value )
		{
			this->m_quickswitch_swap = false;
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || !local.pawn )
		{
			this->m_quickswitch_swap = false;
			return;
		}

		// When the local player fires a weapon this tick, swap to slot 3 (knife)
		// and back to slot 1 on the following tick. This cancels the post-fire
		// delay for weapons like the AWP/SSG and skips the bolt/reload animation.
		if ( features::combat::g_rage.is_firing_this_tick( ) )
		{
			memory::call<void>( PATTERN( patterns::engine_client_cmd ), addresses::globals::source2engine_to_client, 0, xs( "slot3" ), 0x7ffef001 );
			this->m_quickswitch_swap = true;
		}
		else if ( this->m_quickswitch_swap )
		{
			memory::call<void>( PATTERN( patterns::engine_client_cmd ), addresses::globals::source2engine_to_client, 0, xs( "slot1" ), 0x7ffef001 );
			this->m_quickswitch_swap = false;
		}
	}

	void other::do_quick_plant( )
	{
		if ( !settings::g_misc.quick_plant.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || !local.pawn )
		{
			return;
		}

		// Quick plant: while holding a bomb and pressing use near a site, force
		// the plant without the long deploy animation by keeping in_use asserted.
		const auto weapon_services = memory::safe_read<std::uintptr_t>( local.pawn + SCHEMA( "C_CSPlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
		if ( !weapon_services )
		{
			return;
		}

		const auto active_weapon_handle = memory::safe_read<std::uint32_t>( weapon_services + SCHEMA( "C_CSPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
		const auto active_weapon = systems::g_entities.lookup( active_weapon_handle );
		if ( !active_weapon )
		{
			return;
		}

		const auto item_def_idx = memory::safe_read<std::uint16_t>(
			active_weapon
			+ SCHEMA( "C_EconEntity", "m_AttributeManager"_hash )
			+ SCHEMA( "C_AttributeContainer", "m_Item"_hash )
			+ SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) ).value_or( 0 );

		// C4 is item def index 49.
		if ( item_def_idx == 49 )
		{
			const auto cmd = systems::g_input.get( );
			if ( cmd )
			{
				cmd->buttons.value |= cstypes::command_buttons::in_attack;
				cmd->buttons.value_changed |= cstypes::command_buttons::in_attack;
			}
		}
	}

	void other::do_no_land_inaccuracy( )
	{
		if ( !settings::g_misc.no_land_inaccuracy.value )
		{
			this->m_was_on_ground = true;
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || !local.pawn )
		{
			this->m_was_on_ground = true;
			return;
		}

		const auto flags = memory::safe_read<std::uint32_t>( local.pawn + SCHEMA( "C_BaseEntity", "m_fFlags"_hash ) ).value_or( 0 );
		constexpr std::uint32_t on_ground{ 1u << 0 };
		const auto is_on_ground = ( flags & on_ground ) != 0;

		// The moment we transition from airborne to grounded, snap the inaccuracy
		// penalty back to zero by resetting the relevant recoil/accuracy index.
		if ( !this->m_was_on_ground && is_on_ground )
		{
			const auto weapon_services = memory::safe_read<std::uintptr_t>( local.pawn + SCHEMA( "C_CSPlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
			if ( weapon_services )
			{
				const auto active_weapon_handle = memory::safe_read<std::uint32_t>( weapon_services + SCHEMA( "C_CSPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
				const auto active_weapon = systems::g_entities.lookup( active_weapon_handle );
				if ( active_weapon )
				{
					memory::write<float>( active_weapon + SCHEMA( "C_CSWeaponBase", "m_flRecoilIndex"_hash ), 0.0f );
				}
			}
		}

		this->m_was_on_ground = is_on_ground;
	}

	void other::do_sniper_crosshair_patch( )
	{
		const auto should_patch = settings::g_misc.sniper_crosshair.value;

		if ( should_patch == this->m_sniper_crosshair_patched )
		{
			return;
		}

		// Resolve the draw_crosshair pattern once.
		static const auto draw_crosshair_addr = memory::resolve_pattern(
			"client.dll:48895C2408574883EC20488BD9E8????????4885C00F84????????488B05????????8078580074??B001" );
		if ( !draw_crosshair_addr )
		{
			return;
		}

		// The pattern ends at "48 85 C0" (test al, al).  The bytes are:
		//   48 89 5C 24 XX 57 48 83 EC XX 48 8B D9 E8 ?? ?? ?? ?? 48 85 C0
		// That's 19 bytes total.  "48 85 C0" starts at offset 19 - 3 = 16.
		// Immediately after it there should be a conditional jump (e.g. 74 XX
		// for a short JE, or 0F 84 XX XX XX XX for a near JE).  We NOP it
		// so the crosshair is always drawn, even when scoped.
		constexpr auto patch_offset = 19;
		constexpr auto patch_len = 6; // max size for a near-JCC (6 bytes)

		if ( should_patch )
		{
			// Save original bytes.
			for ( auto i = 0; i < patch_len; ++i )
			{
				this->m_patch_original_bytes[ i ] =
					memory::safe_read<std::uint8_t>( draw_crosshair_addr + patch_offset + i ).value_or( 0 );
			}

			// Replace with NOPs.
			DWORD old_protect{};
			VirtualProtect( reinterpret_cast< void* >( draw_crosshair_addr + patch_offset ), patch_len, PAGE_EXECUTE_READWRITE, &old_protect );
			for ( auto i = 0; i < patch_len; ++i )
			{
				memory::write<std::uint8_t>( draw_crosshair_addr + patch_offset + i, 0x90 );
			}
			VirtualProtect( reinterpret_cast< void* >( draw_crosshair_addr + patch_offset ), patch_len, old_protect, &old_protect );

			this->m_sniper_crosshair_patched = true;
		}
		else
		{
			// Restore original bytes.
			DWORD old_protect{};
			VirtualProtect( reinterpret_cast< void* >( draw_crosshair_addr + patch_offset ), patch_len, PAGE_EXECUTE_READWRITE, &old_protect );
			for ( auto i = 0; i < patch_len; ++i )
			{
				memory::write<std::uint8_t>( draw_crosshair_addr + patch_offset + i, this->m_patch_original_bytes[ i ] );
			}
			VirtualProtect( reinterpret_cast< void* >( draw_crosshair_addr + patch_offset ), patch_len, old_protect, &old_protect );

			this->m_sniper_crosshair_patched = false;
		}
	}

} // namespace features::misc
