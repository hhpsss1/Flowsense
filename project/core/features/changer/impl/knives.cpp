#include <pch/pch.hpp>
#include <cstring>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
namespace features::changer {

	void knives::on_frame_stage_notify( )
	{
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

		// The menu edits skins.data on its own thread; snapshot under its mutex
		// so iterating here can never walk a freed node.
		settings::changer::applied_skin selected_skin_copy{};
		const econ_item_system::item_def* selected_knife_def{ nullptr };

		// A profile normally has one knife entry, but old/imported profiles can
		// retain a default-knife entry beside the selected model.  unordered_map
		// iteration made that choice random, which is why switching profiles could
		// fall back to a default knife with the wrong animation set.
		{
			std::lock_guard<std::recursive_mutex> lock{ settings::g_changer.skins.mx };

			for ( const auto& [def_idx, skin] : settings::g_changer.skins.data )
			{
				const auto def = g_econ_item_system.find_def( def_idx );
				if ( !def || def->category != econ_item_system::item_category::knife )
				{
					continue;
				}

				const auto is_default_knife = def->def_index == 42 || def->def_index == 59;
				const auto selected_is_default = selected_knife_def
					&& ( selected_knife_def->def_index == 42 || selected_knife_def->def_index == 59 );

				if ( !selected_knife_def
					|| ( selected_is_default && !is_default_knife )
					|| ( selected_is_default == is_default_knife && def->def_index < selected_knife_def->def_index ) )
				{
					selected_skin_copy = settings::changer::snapshot_skin( skin );
					selected_knife_def = def;
				}
			}
		}

		const settings::changer::applied_skin* selected_skin = &selected_skin_copy;

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
			this->m_original = {};
			this->m_overridden = false;
			this->m_last_active_handle = 0;
			this->m_last_paint_color = false;
			this->m_last_cosmetic = {};
			this->m_tracked_pawn = local.pawn;
		}

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

			if ( !current_def || current_def->category != econ_item_system::item_category::knife )
			{
				continue;
			}

			if ( !memory::read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ) )
			{
				continue;
			}

			if ( !selected_knife_def )
			{
				this->restore( weapon, iv, local.pawn );
				break;
			}

			if ( !this->m_overridden )
			{
				this->capture_original( weapon, iv );
			}

			const auto target_token = detail::make_subclass_token( selected_knife_def->def_index );
			const auto current_subclass = memory::read<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) );
			const auto current_pk = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
			const auto current_seed = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) );
			const auto current_wear = memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) );

			const auto custom_name = iv + SCHEMA( "C_EconItemView", "m_szCustomName"_hash );
			const auto name_matches = ( !settings::changer::valid_nametag( selected_skin->nametag ) || selected_skin->nametag.empty( ) )
				? ( memory::read<char>( custom_name ) == '\0' )
				: ( std::strncmp( selected_skin->nametag.c_str( ), reinterpret_cast< const char* >( custom_name ), 160 ) == 0 );

			const auto cosmetic_matches = this->m_last_cosmetic.handle == handle
				&& this->m_last_cosmetic.config == detail::make_cosmetic_config( *selected_skin );

			const auto config_changed = current_subclass != target_token
				|| current_pk != selected_skin->paint_kit_id
				|| current_seed != selected_skin->seed
				|| current_wear != selected_skin->wear
				|| !name_matches
				|| this->m_last_paint_color != selected_skin->paint_color
				|| !cosmetic_matches;

			if ( !config_changed )
			{
				break;
			}

			this->apply( weapon, iv, selected_knife_def, selected_skin, local.pawn, handle, did_update );
			break;
		}

		if ( did_update )
		{
			detail::regenerate_skins( );
			detail::clear_hud_weapon_icons( );
		}

		if ( active_handle != this->m_last_active_handle )
		{
			this->m_last_active_handle = active_handle;

			if ( this->m_overridden && active_weapon )
			{
				const auto iv = active_weapon + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash ) + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
				const auto def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
				const auto def = g_econ_item_system.find_def( static_cast< std::int16_t >( def_index ) );

				if ( def && def->category == econ_item_system::item_category::knife )
				{
					const auto paint_kit_id = memory::read<int>( active_weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
					const auto pk = g_econ_item_system.find_paint_kit( paint_kit_id );
					this->update_view_model( active_weapon, local.pawn, pk );
				}
			}
		}
	}

	void knives::capture_original( std::uintptr_t weapon, std::uintptr_t iv )
	{
		if ( this->m_original.captured )
		{
			return;
		}

		this->m_original.def_index = memory::read<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
		this->m_original.id_high = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ) );
		this->m_original.id_low = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ) );
		this->m_original.account_id = memory::read<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ) );
		this->m_original.initialized = memory::read<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ) );
		this->m_original.paint_kit = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ) );
		this->m_original.seed = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ) );
		this->m_original.wear = memory::read<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ) );
		this->m_original.stattrak = memory::read<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ) );
		this->m_original.captured = true;
	}

	void knives::apply( std::uintptr_t weapon, std::uintptr_t iv, const econ_item_system::item_def* def, const settings::changer::applied_skin* skin, std::uintptr_t pawn, std::uint32_t handle, bool& did_update )
	{
		// Force item view cache miss - always fall back to weapon-level fields.
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), 0xFFFFFFFF );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), 0 );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), true );
		memory::write<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), static_cast< std::uint16_t >( def->def_index ) );
		memory::write<std::int32_t>( iv + SCHEMA( "C_EconItemView", "m_iEntityQuality"_hash ), 3 );

		// Subclass from the selected knife's def_index
		memory::write<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ), detail::make_subclass_token( def->def_index ) );

		// Fallback fields
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), skin->paint_kit_id );
		memory::write<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), skin->wear );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), skin->seed );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), skin->stattrak ? skin->stattrak_value : -1 );

		// Refresh the model after the fallback fields are written so the
		// precache/material rebuild picks up the new paint kit.
		if ( !def->model_player.empty( ) )
		{
			memory::call<void>( PATTERN( patterns::set_model ), weapon, def->model_player.c_str( ) );

			if ( const auto hud_weapon = detail::get_hud_weapon( weapon, pawn ) )
			{
				memory::call<void>( PATTERN( patterns::set_model ), hud_weapon, def->model_player.c_str( ) );
			}
		}

		const auto pk = g_econ_item_system.find_paint_kit( skin->paint_kit_id );
		const auto uses_old_model = pk && pk->legacy_model;
		const auto mesh_mask = uses_old_model ? std::uint64_t{ 1 } : std::uint64_t{ 2 };

		if ( const auto node = memory::read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ) )
		{
			memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), node, mesh_mask );
		}

		if ( const auto hud_weapon = detail::get_hud_weapon( weapon, pawn ) )
		{
			if ( const auto hud_node = memory::read<std::uintptr_t>( hud_weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ) )
			{
				memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), hud_node, mesh_mask );
			}
		}

		// Profile changes do not change the active-weapon handle.  Force the HUD
		// model refresh path anyway so its animation graph is rebuilt for the new
		// knife model instead of retaining the previous/default sequence.
		this->m_last_active_handle = 0;

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
		detail::sync_keychain( handle, weapon, iv, skin->keychain );

		this->m_last_cosmetic = detail::cosmetic_state{ handle, detail::make_cosmetic_config( *skin ) };

		memory::call<void>( PATTERN( patterns::apply_econ_customization ), weapon, true );
		memory::call_vfunc<void>( weapon, 113, true );
		memory::call_vfunc<void>( weapon, 200 );
		memory::call_vfunc<void>( weapon, 105, true );
		memory::call<void>( PATTERN( patterns::update_subclass ), weapon );
		memory::call<void>( detail::regenerate_weapon_skin_fn( ), weapon, false );

		did_update = true;
		detail::clear_hud_weapon_icon_for( weapon );

		if ( const auto hud_weapon = detail::get_hud_weapon( weapon, pawn ) )
		{
			detail::clear_hud_weapon_icon_for( hud_weapon );
		}

		this->m_overridden = true;
		this->m_last_paint_color = skin->paint_color;
	}

	void knives::restore( std::uintptr_t weapon, std::uintptr_t iv, std::uintptr_t pawn )
	{
		if ( !this->m_overridden || !this->m_original.captured )
		{
			return;
		}

		memory::write<std::uint16_t>( iv + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ), this->m_original.def_index );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDHigh"_hash ), this->m_original.id_high );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iItemIDLow"_hash ), this->m_original.id_low );
		memory::write<std::uint32_t>( iv + SCHEMA( "C_EconItemView", "m_iAccountID"_hash ), this->m_original.account_id );
		memory::write<bool>( iv + SCHEMA( "C_EconItemView", "m_bInitialized"_hash ), this->m_original.initialized );
		*reinterpret_cast< char* >( iv + SCHEMA( "C_EconItemView", "m_szCustomName"_hash ) ) = '\0';

		detail::clear_sticker_keychain_attributes( iv );
		this->m_last_cosmetic = {};

		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackPaintKit"_hash ), this->m_original.paint_kit );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackSeed"_hash ), this->m_original.seed );
		memory::write<float>( weapon + SCHEMA( "C_EconEntity", "m_flFallbackWear"_hash ), this->m_original.wear );
		memory::write<int>( weapon + SCHEMA( "C_EconEntity", "m_nFallbackStatTrak"_hash ), this->m_original.stattrak );

		memory::write<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ), detail::make_subclass_token( static_cast< std::int16_t >( this->m_original.def_index ) ) );

		const auto original_def = g_econ_item_system.find_def( static_cast< std::int16_t >( this->m_original.def_index ) );
		if ( original_def && !original_def->model_player.empty( ) )
		{
			memory::call<void>( PATTERN( patterns::set_model ), weapon, original_def->model_player.c_str( ) );
		}

		const auto pk = g_econ_item_system.find_paint_kit( this->m_original.paint_kit );
		const auto uses_old_model = pk && pk->legacy_model;
		const auto mesh_mask = uses_old_model ? std::uint64_t{ 1 } : std::uint64_t{ 2 };

		if ( const auto node = memory::read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ) )
		{
			memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), node, mesh_mask );
		}

		if ( const auto hud_weapon = detail::get_hud_weapon( weapon, pawn ) )
		{
			if ( const auto hud_node = memory::read<std::uintptr_t>( hud_weapon + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ) )
			{
				memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), hud_node, mesh_mask );
			}

			detail::clear_hud_weapon_icon_for( hud_weapon );
		}

		// Notify game of customization changes (same as apply() does)
		memory::call<void>( PATTERN( patterns::apply_econ_customization ), weapon, true );
		memory::call_vfunc<void>( weapon, 113, true );
		memory::call_vfunc<void>( weapon, 200 );
		memory::call_vfunc<void>( weapon, 105, true );
		memory::call<void>( PATTERN( patterns::update_subclass ), weapon );
		memory::call<void>( detail::regenerate_weapon_skin_fn( ), weapon, false );

		// Note: detail::regenerate_skins() and clear_hud_weapon_icons() are called
		// by on_frame_stage_notify() after restore() sets did_update = true

		this->m_overridden = false;
	}

	void knives::update_view_model( std::uintptr_t weapon, std::uintptr_t pawn, const econ_item_system::paint_kit* pk )
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
		memory::call<void>( PATTERN( patterns::weapon_set_mesh_group_mask ), view_model_scene_node, uses_old_model ? std::uint64_t{ 1 } : std::uint64_t{ 2 } );
	}

} // namespace features::changer
