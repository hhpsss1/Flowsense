#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <core/rendering/rendering.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <core/systems/systems.hpp>

namespace features::esp::item {

	namespace detail {

		// Offsets verified in pastehook (verified_features.hpp / client_dll.hpp):
		// CEntityInstance::m_pEntity = 0x10, CEntityIdentity::m_designerName = 0x20.
		constexpr auto k_identity_offset = 0x10;
		constexpr auto k_designer_name_offset = 0x20;

		// pastehook: CEntityIdentity::m_designerName, e.g. "weapon_ak47".
		[[nodiscard]] std::string get_designer_name( std::uintptr_t entity )
		{
			const auto identity = memory::safe_read<std::uintptr_t>( entity + k_identity_offset ).value_or( 0 );
			if ( !identity || identity < 0x10000 )
			{
				return {};
			}

			const auto name_ptr = memory::safe_read<std::uintptr_t>( identity + k_designer_name_offset ).value_or( 0 );
			if ( !name_ptr || name_ptr < 0x10000 )
			{
				return {};
			}

			return memory::read_string( name_ptr, 64 );
		}

		// pastehook's normalize_weapon_name: lowercase and strip weapon_/item_/c_.
		[[nodiscard]] std::string normalize_weapon_name( std::string name )
		{
			if ( name.empty( ) )
			{
				return {};
			}

			for ( auto& c : name )
			{
				c = ( c >= 'A' && c <= 'Z' ) ? static_cast< char >( c - 'A' + 'a' ) : c;
			}

			const char* prefixes[ ] = { "weapon_", "item_", "c_" };
			for ( const auto prefix : prefixes )
			{
				const auto len = std::strlen( prefix );
				if ( name.rfind( prefix, 0 ) == 0 )
				{
					name.erase( 0, len );
					break;
				}
			}

			return name;
		}

		// pastehook's is_grenade_weapon_entity: keyword match on the designer name,
		// excluding anything that is an actual projectile entity.
		[[nodiscard]] bool is_grenade_weapon_name( const std::string& designer )
		{
			if ( designer.find( "projectile" ) != std::string::npos )
			{
				return false;
			}

			for ( const auto* keyword : { "flashbang", "hegrenade", "smokegrenade", "molotov", "decoy", "incgrenade", "incendiary" } )
			{
				if ( designer.find( keyword ) != std::string::npos )
				{
					return true;
				}
			}

			return false;
		}

		// pastehook: whether the entity at all looks like a weapon drop.
		[[nodiscard]] bool is_weapon_entity( const std::string& designer )
		{
			if ( designer.empty( ) )
			{
				return false;
			}

			if ( designer.find( "weapon_" ) != std::string::npos )
			{
				return true;
			}

			return is_grenade_weapon_name( designer );
		}

		// pastehook's weapon category groups (0-5, mirroring the settings layout).
		[[nodiscard]] std::uint32_t group_from_key( const std::string& key )
		{
			const auto find_case = []( std::string_view haystack, std::string_view needle )
			{
				return haystack.find( needle ) != std::string_view::npos;
			};

			if ( find_case( key, "knife" ) || find_case( key, "bayonet" ) )
			{
				return 5;
			}

			switch ( fnv1a::runtime_hash( key.c_str( ) ) )
			{
			case "deagle"_hash:
			case "elite"_hash:
			case "fiveseven"_hash:
			case "glock"_hash:
			case "hkp2000"_hash:
			case "usp_silencer"_hash:
			case "p250"_hash:
			case "cz75a"_hash:
			case "tec9"_hash:
			case "revolver"_hash:
				return 0;

			case "mac10"_hash:
			case "mp5sd"_hash:
			case "mp7"_hash:
			case "mp9"_hash:
			case "bizon"_hash:
			case "p90"_hash:
			case "ump45"_hash:
				return 1;

			case "ak47"_hash:
			case "m4a1"_hash:
			case "m4a1_silencer"_hash:
			case "aug"_hash:
			case "famas"_hash:
			case "galilar"_hash:
			case "sg556"_hash:
				return 2;

			case "nova"_hash:
			case "sawedoff"_hash:
			case "xm1014"_hash:
			case "mag7"_hash:
				return 3;

			case "awp"_hash:
			case "g3sg1"_hash:
			case "scar20"_hash:
			case "ssg08"_hash:
			case "m249"_hash:
			case "negev"_hash:
				return 4;

			case "hegrenade"_hash:
			case "flashbang"_hash:
			case "smokegrenade"_hash:
			case "molotov"_hash:
			case "incgrenade"_hash:
			case "decoy"_hash:
			case "c4"_hash:
			case "taser"_hash:
			case "healthshot"_hash:
				return 5;

			default:
				// Anything still recognizable as a weapon falls back to utility so a
				// label is always produced instead of silently skipping (pastehook
				// has no per-group filtering and never refuses to draw).
				return 5;
			}
		}

		// pastehook's weapon_icons::display_table, used for the text label.
		[[nodiscard]] std::string display_name( const std::string& key )
		{
			if ( key.empty( ) )
			{
				return {};
			}

			const static std::unordered_map<std::string, const char*> display_table
			{
				{ "deagle", "Desert Eagle" },
				{ "elite", "Dual Berettas" },
				{ "fiveseven", "Five-SeveN" },
				{ "glock", "Glock-18" },
				{ "ak47", "AK-47" },
				{ "aug", "AUG" },
				{ "awp", "AWP" },
				{ "famas", "FAMAS" },
				{ "g3sg1", "G3SG1" },
				{ "galilar", "Galil AR" },
				{ "m249", "M249" },
				{ "m4a1", "M4A4" },
				{ "mac10", "MAC-10" },
				{ "p90", "P90" },
				{ "mp5sd", "MP5-SD" },
				{ "ump45", "UMP-45" },
				{ "xm1014", "XM1014" },
				{ "bizon", "PP-Bizon" },
				{ "mag7", "MAG-7" },
				{ "negev", "Negev" },
				{ "sawedoff", "Sawed-Off" },
				{ "tec9", "Tec-9" },
				{ "taser", "Zeus x27" },
				{ "hkp2000", "P2000" },
				{ "mp7", "MP7" },
				{ "mp9", "MP9" },
				{ "nova", "Nova" },
				{ "p250", "P250" },
				{ "shield", "Shield" },
				{ "scar20", "SCAR-20" },
				{ "sg556", "SG 553" },
				{ "ssg08", "SSG 08" },
				{ "knife", "Knife" },
				{ "knife_gg", "Knife" },
				{ "flashbang", "Flashbang" },
				{ "hegrenade", "HE Grenade" },
				{ "smokegrenade", "Smoke Grenade" },
				{ "molotov", "Molotov" },
				{ "decoy", "Decoy" },
				{ "incgrenade", "Incendiary" },
				{ "c4", "Bomb" },
				{ "knife_t", "Knife (T)" },
				{ "m4a1_silencer", "M4A1-S" },
				{ "usp_silencer", "USP-S" },
				{ "cz75a", "CZ75-Auto" },
				{ "revolver", "R8 Revolver" },
				{ "knife_bayonet", "Bayonet" },
				{ "knife_css", "Knife" },
				{ "knife_flip", "Flip Knife" },
				{ "knife_gut", "Gut Knife" },
				{ "knife_karambit", "Karambit" },
				{ "knife_m9_bayonet", "M9 Bayonet" },
				{ "knife_tactical", "Tactical Knife" },
				{ "knife_falchion", "Falchion Knife" },
				{ "knife_survival_bowie", "Bowie Knife" },
				{ "knife_butterfly", "Butterfly Knife" },
				{ "knife_push", "Shadow Daggers" },
				{ "knife_cord", "Cord Knife" },
				{ "knife_canis", "Canis Knife" },
				{ "knife_ursus", "Ursus Knife" },
			};

			const auto it = display_table.find( key );
			if ( it != display_table.end( ) && it->second && it->second[ 0 ] )
			{
				return it->second;
			}

			return key;
		}

		// Mirrors pastehook's is_weapon_in_player_inventory: a weapon is considered
		// held when it appears in a player's m_hMyWeapons list or is the active weapon.
		[[nodiscard]] bool weapon_in_services( std::uintptr_t weapon, std::uintptr_t pawn )
		{
			if ( !weapon || !pawn )
			{
				return false;
			}

			const auto weapon_services = memory::safe_read<std::uintptr_t>( pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
			if ( !weapon_services )
			{
				return false;
			}

			const auto weapons_base = weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hMyWeapons"_hash );
			const auto size = memory::safe_read<int>( weapons_base ).value_or( 0 );
			if ( size <= 0 || size > 64 )
			{
				return false;
			}

			const auto list = memory::safe_read<std::uintptr_t>( weapons_base + 0x8 ).value_or( 0 );
			if ( !list )
			{
				return false;
			}

			for ( auto i = 0; i < size; ++i )
			{
				const auto handle = memory::safe_read<std::uint32_t>( list + static_cast< std::uintptr_t >( i ) * sizeof( std::uint32_t ) ).value_or( 0 );
				if ( handle && handle != 0xffffffff && systems::g_entities.lookup( handle ) == weapon )
				{
					return true;
				}
			}

			return false;
		}

		// pastehook's is_dropped_weapon (negated): a weapon is held when any player
		// carries it in inventory or it exposes a valid owner handle.
		[[nodiscard]] bool is_held( std::uintptr_t weapon )
		{
			if ( !weapon )
			{
				return false;
			}

			if ( const auto local = systems::g_local.get( ).pawn; local && weapon_in_services( weapon, local ) )
			{
				return true;
			}

			for ( const auto& player : systems::g_entities.get_by_type( systems::entities::type::player ) )
			{
				const auto pawn_handle = memory::safe_read<std::uint32_t>( player.ptr + SCHEMA( "CBasePlayerController", "m_hPawn"_hash ) ).value_or( 0 );
				if ( !pawn_handle || pawn_handle == 0xffffffff )
				{
					continue;
				}

				const auto pawn = systems::g_entities.lookup( pawn_handle );
				if ( pawn && weapon_in_services( weapon, pawn ) )
				{
					return true;
				}
			}

			const auto owner_handle = memory::safe_read<std::uint32_t>( weapon + SCHEMA( "C_BaseEntity", "m_hOwnerEntity"_hash ) ).value_or( 0 );
			if ( owner_handle && ( owner_handle & 0x7fff ) != 0x7fff )
			{
				return true;
			}

			return false;
		}

	} // namespace detail

	void overlay::on_render( xdraw::draw_list& draw_list )
	{
		const auto& overlay_cfg = settings::g_esp.m_item.m_overlay;
		if ( !overlay_cfg.enabled.value )
		{
			return;
		}

		// Scan the raw entity list exactly like pastehook's weapon drop loop, and
		// additionally fall back to cached items in case the list stride changed.
		std::unordered_set<std::uintptr_t> seen{};

		std::uint32_t stat_entities{ 0 };
		std::uint32_t stat_weapons{ 0 };
		std::uint32_t stat_dropped{ 0 };
		std::uint32_t stat_drawn{ 0 };

		const auto process = [ & ]( std::uintptr_t ptr, std::uint8_t group_hint )
		{
			if ( !ptr || !seen.emplace( ptr ).second )
			{
				return;
			}

			++stat_entities;

			const auto designer = detail::get_designer_name( ptr );
			if ( designer.empty( ) || !detail::is_weapon_entity( designer ) )
			{
				return;
			}

			++stat_weapons;

			const auto key = detail::normalize_weapon_name( designer );
			if ( key.empty( ) )
			{
				return;
			}

			const auto group_id = group_hint == UINT8_MAX ? detail::group_from_key( key ) : static_cast< std::uint32_t >( group_hint );

			const systems::entities::cached entity{ ptr, fnv1a::runtime_hash( designer.c_str( ) ), static_cast< std::int16_t >( -1 ), systems::entities::type::item };

			const auto info = this->get_info( entity, key );
			if ( !info.valid( ) )
			{
				return;
			}

			if ( detail::is_held( ptr ) )
			{
				return;
			}

			++stat_dropped;

			if ( !overlay_cfg.is_active( group_id ) || info.distance > overlay_cfg.get_group( group_id ).max_distance )
			{
				return;
			}

			this->add_label( draw_list, info, overlay_cfg.get_group( group_id ) );
			++stat_drawn;
		};

		for ( auto i = 0; i < 2048; ++i )
		{
			process( systems::g_entities.get_by_index( i ), UINT8_MAX );
		}

		for ( const auto& cached : systems::g_entities.get_by_type( systems::entities::type::item ) )
		{
			process( cached.ptr, UINT8_MAX );
		}

		// Diagnostics: printed once every few seconds so a non-drawing setup can be
		// traced through every filter stage via the diagnostics file.
		static auto last_log{ std::chrono::steady_clock::now( ) };
		const auto now = std::chrono::steady_clock::now( );
		if ( now - last_log > std::chrono::seconds( 3 ) )
		{
			last_log = now;

			char buffer[ 160 ];
			std::snprintf( buffer, sizeof( buffer ), "item overlay: entities=%u weapons=%u dropped=%u drawn=%u", stat_entities, stat_weapons, stat_dropped, stat_drawn );
			diag::write( diag::level::info, buffer );
		}
	}

	void overlay::add_label( xdraw::draw_list& draw_list, const info& info, const settings::esp::item::overlay::group& cfg )
	{
		const auto screen = systems::g_view.project( info.origin );
		if ( !systems::g_view.projection_valid( screen ) )
		{
			return;
		}

		const auto wants_icon = cfg.display == settings::esp::item::overlay::group::display_type::icon || cfg.display == settings::esp::item::overlay::group::display_type::text_and_icon;
		const auto wants_text = cfg.display == settings::esp::item::overlay::group::display_type::text || cfg.display == settings::esp::item::overlay::group::display_type::text_and_icon;

		const auto ico = wants_icon ? systems::g_icons.get( info.name, 0.35f ) : nullptr;
		const auto icon_ok = ico && ico->texture;

		// Fall back to text whenever an icon was requested but is unavailable, so an
		// icon-only display never silently renders nothing.
		const auto show_icon = icon_ok;
		const auto show_text = wants_text || ( wants_icon && !icon_ok );
		auto y = screen.y;

		if ( show_icon )
		{
			const auto iw = static_cast< float >( ico->width );
			const auto ih = static_cast< float >( ico->height );
			const auto ix = std::floorf( screen.x - iw * 0.5f );
			const auto iy = std::floorf( y );

			// All icons render as solid white, no border/outline.
			constexpr auto white{ xdraw::color{ 255, 255, 255, 255 } };
			draw_list.image( ix, iy, iw, ih, ico->texture.Get( ), white );

			y += ih + 1.0f;
		}

		if ( show_text )
		{
			xdraw::push_font( rendering::g_fonts.inter_medium[ rendering::fonts::size::petite ] );

			const auto label = detail::display_name( info.name );
			const auto [w, h] = xdraw::measure_text( label );
			draw_list.text( std::floorf( screen.x - w * 0.5f ), std::floorf( y ), label, cfg.text_color, xdraw::text_style::outlined );

			xdraw::pop_font( );
		}
	}

	overlay::info overlay::get_info( const systems::entities::cached& entity, const std::string& name )
	{
		info info{};
		info.entity = entity.ptr;
		info.schema_hash = entity.schema_hash;
		info.name = name;

		if ( !info.entity )
		{
			return info;
		}

		const auto game_scene_node = memory::safe_read<std::uintptr_t>( info.entity + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
		if ( !game_scene_node )
		{
			info.name.clear( );
			return info;
		}

		info.origin = memory::safe_read<math::vector3>( game_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) ).value_or( math::vector3{} );
		info.distance = systems::g_view.origin( ).distance( info.origin ) * 0.01905f;

		return info;
	}

	std::uint32_t overlay::get_item_group( std::uint32_t schema_hash )
	{
		switch ( schema_hash )
		{
		case "C_DEagle"_hash:
		case "C_WeaponElite"_hash:
		case "C_WeaponFiveSeven"_hash:
		case "C_WeaponGlock"_hash:
		case "C_WeaponHKP2000"_hash:
		case "C_WeaponUSPSilencer"_hash:
		case "C_WeaponP250"_hash:
		case "C_WeaponCZ75a"_hash:
		case "C_WeaponTec9"_hash:
		case "C_WeaponRevolver"_hash:
			return 0;

		case "C_WeaponMAC10"_hash:
		case "C_WeaponMP5SD"_hash:
		case "C_WeaponMP7"_hash:
		case "C_WeaponMP9"_hash:
		case "C_WeaponBizon"_hash:
		case "C_WeaponP90"_hash:
		case "C_WeaponUMP45"_hash:
			return 1;

		case "C_AK47"_hash:
		case "C_WeaponM4A1"_hash:
		case "C_WeaponM4A1Silencer"_hash:
		case "C_WeaponAug"_hash:
		case "C_WeaponFamas"_hash:
		case "C_WeaponGalilAR"_hash:
		case "C_WeaponSG556"_hash:
			return 2;

		case "C_WeaponNOVA"_hash:
		case "C_WeaponSawedoff"_hash:
		case "C_WeaponXM1014"_hash:
		case "C_WeaponMag7"_hash:
			return 3;

		case "C_WeaponAWP"_hash:
		case "C_WeaponG3SG1"_hash:
		case "C_WeaponSCAR20"_hash:
		case "C_WeaponSSG08"_hash:
		case "C_WeaponM249"_hash:
		case "C_WeaponNegev"_hash:
			return 4;

		case "C_HEGrenade"_hash:
		case "C_Flashbang"_hash:
		case "C_SmokeGrenade"_hash:
		case "C_MolotovGrenade"_hash:
		case "C_IncendiaryGrenade"_hash:
		case "C_DecoyGrenade"_hash:
		case "C_C4"_hash:
		case "C_WeaponTaser"_hash:
		case "C_Item_Healthshot"_hash:
		case "C_Knife"_hash:
			return 5;

		default:
			return UINT32_MAX;
		}
	}

} // namespace features::esp::item