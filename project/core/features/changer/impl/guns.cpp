#include <pch/pch.hpp>
#include <cstring>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
namespace features::changer {

	namespace {



		void dump_bytes( const char* label, std::uintptr_t address, std::size_t length )
		{
			if ( !address )
			{
				logging::console::print( xs( "[dump] {} = 0x0" ), label );
				return;
			}

			__try
			{
				std::string hex;
				hex.reserve( length * 3 );

				for ( auto i = 0ull; i < length; ++i )
				{
					char byte_buf[ 4 ];
					std::snprintf( byte_buf, sizeof( byte_buf ), "%02X ", *reinterpret_cast<std::uint8_t*>( address + i ) );
					hex += byte_buf;
				}

				logging::console::print( xs( "[dump] {} @ {:#x}: {}" ), label, address, hex );
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				logging::console::print( xs( "[dump] {} read fault" ), label );
			}
		}

		void list_call_targets( const char* label, std::uintptr_t address, std::size_t length )
		{
			if ( !address )
			{
				return;
			}

			__try
			{
				for ( auto i = 0ull; i + 5 <= length; ++i )
				{
					if ( *reinterpret_cast<std::uint8_t*>( address + i ) != 0xE8 )
					{
						continue;
					}

					const auto rel = *reinterpret_cast<std::int32_t*>( address + i + 1 );
					const auto target = address + i + 5 + rel;
					logging::console::print( xs( "[dump] {} call @ +0x{:04x} -> {:#x}" ), label, i, target );
				}
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				logging::console::print( xs( "[dump] {} scan fault" ), label );
			}
		}

		void log_pattern_status( )
		{
			static const auto once = [] ( )
			{
				logging::console::print( xs( "[skinchanger] apply_econ_customization    = {:#x}" ), PATTERN( patterns::apply_econ_customization ) );
				logging::console::print( xs( "[skinchanger] add_keychain_entity        = {:#x}" ), PATTERN( patterns::add_keychain_entity ) );
				logging::console::print( xs( "[skinchanger] add_nametag_entity         = {:#x}" ), PATTERN( patterns::add_nametag_entity ) );
				logging::console::print( xs( "[skinchanger] add_stattrak_entity       = {:#x}" ), PATTERN( patterns::add_stattrak_entity ) );
				logging::console::print( xs( "[skinchanger] weapon_update_composite    = {:#x}" ), PATTERN( patterns::weapon_update_composite_material ) );
				logging::console::print( xs( "[skinchanger] update_subclass            = {:#x}" ), PATTERN( patterns::update_subclass ) );
				logging::console::print( xs( "[skinchanger] build_legacy_skin_material  = {:#x}" ), PATTERN( patterns::build_legacy_weapon_skin_material ) );
				logging::console::print( xs( "[skinchanger] build_modern_skin_material  = {:#x}" ), PATTERN( patterns::build_modern_weapon_skin_material ) );
				logging::console::print( xs( "[skinchanger] regenerate_weapon_skins    = {:#x}" ), PATTERN( patterns::regenerate_weapon_skins ) );
				logging::console::print( xs( "[skinchanger] derived weapon skin regen  = {:#x}" ), detail::regenerate_weapon_skin_fn( ) );
				logging::console::print( xs( "[skinchanger] clear_hud_weapon_icon      = {:#x}" ), PATTERN( patterns::clear_hud_weapon_icon ) );
				logging::console::print( xs( "[skinchanger] set_model                  = {:#x}" ), PATTERN( patterns::set_model ) );
				logging::console::print( xs( "[skinchanger] find_hud_element           = {:#x}" ), PATTERN( patterns::find_hud_element ) );

				dump_bytes( xs( "regenerate_weapon_skins" ), PATTERN( patterns::regenerate_weapon_skins ), 176 );
				list_call_targets( xs( "regenerate_weapon_skins" ), PATTERN( patterns::regenerate_weapon_skins ), 320 );

				dump_bytes( xs( "apply_econ_customization" ), PATTERN( patterns::apply_econ_customization ), 176 );
				list_call_targets( xs( "apply_econ_customization" ), PATTERN( patterns::apply_econ_customization ), 320 );

				dump_bytes( xs( "update_subclass" ), PATTERN( patterns::update_subclass ), 96 );
				list_call_targets( xs( "update_subclass" ), PATTERN( patterns::update_subclass ), 128 );
				return true;
			} ( );

			( void ) once;
		}

	} // namespace

	void guns::on_frame_stage_notify( )
	{
		log_pattern_status( );

		const auto local = systems::g_local.get( );
		if ( !local.is_alive || systems::g_local.is_in_cinematic( ) || !local.pawn || !local.controller )
		{
			return;
		}

		const auto weapon_services = memory::read<std::uintptr_t>( local.pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) );
		if ( !weapon_services )
		{
			return;
		}

		const auto weapons_base = weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
		const auto weapons_size = memory::read<int>( weapons_base );
		const auto weapons_data = memory::read<std::uintptr_t>( weapons_base + 0x8 );

		if ( !weapons_data || weapons_size <= 0 )
		{
			return;
		}

		const auto active_handle = memory::read<std::uint32_t>( weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) );
		const auto active_weapon = systems::g_entities.lookup( active_handle );

		if ( this->m_tracked_pawn != local.pawn )
		{
			this->m_last_active_handle = 0;
			this->m_tracked_pawn = local.pawn;
		}

		// Get the local player's account ID to check weapon ownership
		const auto local_account_id = memory::read<std::uint32_t>( local.controller + SCHEMA( "CBasePlayerController", "m_iAccountID"_hash ) );

		auto did_update = false;

		for ( auto i = 0; i < weapons_size; ++i )
		{
			const auto handle = memory::read<std::uint32_t>( weapons_data + i * sizeof( std::uint32_t ) );
			const auto weapon = systems::g_entities.lookup( handle );

			if ( !weapon )
			{
				continue;
			}

			const auto iv = weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
			const auto current_def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
			const auto current_def = g_econ_item_system.find_def( static_cast< std::int16_t >( current_def_index ) );

			if ( !current_def || current_def->category != econ_item_system::item_category::gun )
			{
				continue;
			}

			// Skip weapons picked up from enemies - only apply skins to weapons we bought/own
			const auto weapon_account_id = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) );
			if ( weapon_account_id != 0 && weapon_account_id != local_account_id )
			{
				continue;
			}

			// Snapshot under the menu's mutex; the pointer is used by apply()
			// after the scope closes.
			settings::changer::applied_skin skin{};

			{
				std::lock_guard<std::recursive_mutex> lock{ settings::g_changer.skins.mx };

				const auto skin_it = settings::g_changer.skins.data.find( current_def_index );
				if ( skin_it == settings::g_changer.skins.data.end( ) )
				{
					continue;
				}

				skin = settings::changer::snapshot_skin( skin_it->second );
			}

			const auto paint_kit_id = skin.paint_kit_id;

			if ( paint_kit_id == 0 && !skin.paint_color )
			{
				continue;
			}

			if ( !memory::read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ) )
			{
				continue;
			}

			const auto current_paint = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
			const auto current_wear = memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) );
			const auto current_seed = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) );

			// Drive the charm state machine every tick: offset/seed-only edits
			// clear needs_update as soon as apply() refreshes the cosmetic
			// cache, and the deferred remove->respawn sequence has to keep
			// progressing even when nothing else changed.
			detail::sync_keychain( handle, weapon, iv, skin.keychain );

			const auto custom_name = iv + SCHEMA( "C_EconItemView", "m_szCustomName"_hash );
			const auto name_matches = ( !settings::changer::valid_nametag( skin.nametag ) || skin.nametag.empty( ) )
				? ( memory::read<char>( custom_name ) == '\0' )
				: ( std::strncmp( skin.nametag.c_str( ), reinterpret_cast< const char* >( custom_name ), 160 ) == 0 );

			const auto it_paint = this->m_last_paint_color.find( static_cast< std::int16_t >( current_def_index ) );
			const auto last_paint_color = ( it_paint != this->m_last_paint_color.end( ) ) ? it_paint->second : false;

			const auto it_cosmetic = this->m_last_cosmetic.find( static_cast< std::int16_t >( current_def_index ) );
			const auto cosmetic_matches = ( it_cosmetic != this->m_last_cosmetic.end( ) )
				&& it_cosmetic->second.handle == handle
				&& it_cosmetic->second.config == detail::make_cosmetic_config( skin );

			const auto needs_update = current_paint != paint_kit_id || current_wear != skin.wear || current_seed != skin.seed
				|| !name_matches || last_paint_color != skin.paint_color || !cosmetic_matches;
			if ( !needs_update )
			{
				continue;
			}

			this->apply( weapon, iv, local.pawn, &skin, static_cast< std::int16_t >( current_def_index ), handle, did_update );
		}

		if ( did_update )
		{
			detail::regenerate_skins( );
			detail::clear_hud_weapon_icons( );
		}

		if ( active_handle != this->m_last_active_handle )
		{
			this->m_last_active_handle = active_handle;

			if ( active_weapon )
			{
				const auto iv = active_weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
				const auto def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
				const auto def = g_econ_item_system.find_def( static_cast< std::int16_t >( def_index ) );

				if ( def && def->category == econ_item_system::item_category::gun )
				{
					const auto paint_kit_id = memory::read<int>( active_weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
					const auto pk = g_econ_item_system.find_paint_kit( paint_kit_id );
					this->update_view_model( active_weapon, local.pawn, pk );
				}
			}
		}
	}

	void guns::apply( std::uintptr_t weapon, std::uintptr_t iv, std::uintptr_t pawn, const settings::changer::applied_skin* skin, std::int16_t def_index, std::uint32_t handle, bool& did_update )
	{
		logging::console::print( xs( "[kcdbg] apply def={} paint={} kc_id={} kc_seed={} off=({:.2f},{:.2f},{:.2f})" ), def_index, skin->paint_kit_id, skin->keychain.id, skin->keychain.seed, skin->keychain.offset_x, skin->keychain.offset_y, skin->keychain.offset_z );

		if ( skin->paint_kit_id > 0 )
		{
			// Subclass must match the weapon's own def_index
			memory::write<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ), detail::make_subclass_token( def_index ) );
			memory::call<void>( PATTERN( patterns::update_subclass ), weapon );

			// Fallback fields
			memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), skin->paint_kit_id );
			memory::write<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), skin->wear );
			memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), skin->seed );
			memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), skin->stattrak ? skin->stattrak_value : -1 );

			// Item fields - force EconItemView lookup miss - fallback path
			memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), 0xFFFFFFFF );
			memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), 0 );
			memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), true );

			const auto custom_name = iv + SCHEMA( "C_EconItemView", "m_szCustomName"_hash );
			if ( !settings::changer::valid_nametag( skin->nametag ) || skin->nametag.empty( ) )
			{
				*reinterpret_cast< char* >( custom_name ) = '\0';
			}
			else
			{
				strcpy_s( reinterpret_cast< char* >( custom_name ), 161, skin->nametag.c_str( ) );
				detail::add_nametag_entity( weapon, iv );
			}

			// Re-apply the model after the fallback fields are written so the
			// precache/material refresh picks up the new paint kit.
			if ( const auto def = g_econ_item_system.find_def( def_index ) )
			{
				if ( !def->model_player.empty( ) )
				{
					memory::call<void>( PATTERN( patterns::set_model ), weapon, def->model_player.c_str( ) );
				}
			}

			detail::apply_sticker_keychain_attributes( iv, *skin );
			detail::apply_stattrak_attributes( iv, skin->stattrak, skin->stattrak_value );

			memory::write<std::int32_t>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), skin->stattrak ? 3 : 0 );
			if ( skin->stattrak )
			{
				detail::add_stattrak_entity( weapon, iv );
			}

			// Deferred-spawn aware charm sync (handles id=0 cleanup too).
			if ( skin->keychain.id != 0 )
			{
				detail::update_composite_material( weapon );
			}
			this->m_last_cosmetic[ def_index ] = detail::cosmetic_state{ handle, detail::make_cosmetic_config( *skin ) };

			// Call chain
			memory::call<void>( PATTERN( patterns::apply_econ_customization ), weapon, true );
			memory::call_vfunc<void>( weapon, 113, true );
			memory::call_vfunc<void>( weapon, 200 );
			memory::call_vfunc<void>( weapon, 105, true );
			memory::call<void>( PATTERN( patterns::update_subclass ), weapon );
			memory::call<void>( detail::regenerate_weapon_skin_fn( ), weapon, false );

			// Mesh mask from paint kit's old/new model flag
			const auto pk = g_econ_item_system.find_paint_kit( skin->paint_kit_id );
			const auto uses_old_model = pk && pk->legacy_model;
			const auto mesh_mask = 1 + static_cast< std::uint64_t >( uses_old_model );

			if ( const auto node = memory::read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ) )
			{
				memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), node, mesh_mask );
			}

			if ( const auto hud = detail::get_hud_weapon( weapon, pawn ) )
			{
				if ( const auto hud_node = memory::read<std::uintptr_t>( hud + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ) )
				{
					memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), hud_node, mesh_mask );
				}
			}

			detail::clear_hud_weapon_icon_for( weapon );
			this->m_last_paint_color[ def_index ] = skin->paint_color;
			did_update = true;
		}
		else if ( skin->paint_color )
		{
			memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), skin->seed );

			const auto custom_name = iv + SCHEMA( "C_EconItemView", "m_szCustomName"_hash );
			if ( !settings::changer::valid_nametag( skin->nametag ) || skin->nametag.empty( ) )
			{
				*reinterpret_cast< char* >( custom_name ) = '\0';
			}
			else
			{
				strcpy_s( reinterpret_cast< char* >( custom_name ), 161, skin->nametag.c_str( ) );
				detail::add_nametag_entity( weapon, iv );
			}

			detail::apply_sticker_keychain_attributes( iv, *skin );
			detail::apply_stattrak_attributes( iv, skin->stattrak, skin->stattrak_value );

			memory::write<std::int32_t>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), skin->stattrak ? 3 : 0 );
			if ( skin->stattrak )
			{
				detail::add_stattrak_entity( weapon, iv );
			}

			// Deferred-spawn aware charm sync (handles id=0 cleanup too).
			if ( skin->keychain.id != 0 )
			{
				detail::update_composite_material( weapon );
			}
			this->m_last_cosmetic[ def_index ] = detail::cosmetic_state{ handle, detail::make_cosmetic_config( *skin ) };

			memory::call<void>( PATTERN( patterns::apply_econ_customization ), weapon, true );
			memory::call_vfunc<void>( weapon, 113, true );
			memory::call_vfunc<void>( weapon, 105, true );
			memory::call<void>( detail::regenerate_weapon_skin_fn( ), weapon, true );

			this->m_last_paint_color[ def_index ] = skin->paint_color;
			did_update = true;
		}
	}

	void guns::update_view_model( std::uintptr_t weapon, std::uintptr_t pawn, const econ_item_system::paint_kit* pk )
	{
		const auto view_model = detail::get_hud_weapon( weapon, pawn );
		if ( !view_model )
		{
			return;
		}

		const auto view_model_scene_node = memory::read<std::uintptr_t>( view_model + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
		if ( !view_model_scene_node )
		{
			return;
		}

		const auto uses_old_model = pk && pk->legacy_model;
		memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), view_model_scene_node, 1 + static_cast< std::uint64_t >( uses_old_model ) );
	}

} // namespace features::changer