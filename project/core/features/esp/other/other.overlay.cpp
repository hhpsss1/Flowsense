#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/steam/steam.hpp>
#include <core/rendering/rendering.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

namespace features::esp::other {

	namespace detail {

		struct avatar_cache
		{
			struct entry
			{
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture{};
				bool attempted{};
			};

			std::unordered_map<std::uintptr_t, entry> m_entries{};

			[[nodiscard]] ID3D11ShaderResourceView* get( std::uintptr_t steam_id )
			{
				auto it = this->m_entries.find( steam_id );
				if ( it != this->m_entries.end( ) )
				{
					return it->second.texture.Get( );
				}

				auto& e = this->m_entries[ steam_id ];
				e.attempted = true;

				const auto image_handle = steam::friends::get_medium_friend_avatar( steam_id );
				if ( image_handle <= 0 )
				{
					return nullptr;
				}

				std::uint32_t w{}, h{};
				if ( !steam::utils::get_image_size( image_handle, &w, &h ) || !w || !h )
				{
					return nullptr;
				}

				std::vector<std::uint8_t> rgba( w * h * 4 );
				if ( !steam::utils::get_image_rgba( image_handle, rgba.data( ), static_cast< int >( rgba.size( ) ) ) )
				{
					return nullptr;
				}

				e.texture = xdraw::create_srv_from_rgba( rgba.data( ), static_cast< int >( w ), static_cast< int >( h ) );
				return e.texture.Get( );
			}

			void clear( )
			{
				this->m_entries.clear( );
			}
		};

	} // namespace detail

	void overlay::on_render( xdraw::draw_list& draw_list )
	{
		this->add_spectators( draw_list );
		this->add_bomb( draw_list );
	}

	void overlay::add_bomb( xdraw::draw_list& draw_list )
	{
		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || !systems::g_entities.exists( local.view_controller( ) ) )
		{
			return;
		}

		const auto planted_c4 = memory::read<std::uintptr_t>( addresses::globals::planted_c4 );
		const auto global_vars = memory::read<std::uintptr_t>( addresses::globals::global_vars );

		const auto menu_open = rendering::g_menu.is_open( );
		const auto has_bomb = planted_c4 != 0 && global_vars != 0;

		if ( !has_bomb && !menu_open )
		{
			return;
		}

		auto current_time = 0.0f;
		auto blow_time = 40.0f;
		auto has_exploded = false;
		auto bomb_defused = false;
		auto bomb_site = 0;
		auto being_defused = false;
		auto timer_length = 40.0f;

		if ( has_bomb )
		{
			current_time = memory::read<float>( global_vars + 0x30 );
			blow_time = memory::read<float>( planted_c4 + SCHEMA( "C_PlantedC4", "m_flC4Blow"_hash ) );
			has_exploded = memory::read<bool>( planted_c4 + SCHEMA( "C_PlantedC4", "m_bHasExploded"_hash ) );
			bomb_defused = memory::read<bool>( planted_c4 + SCHEMA( "C_PlantedC4", "m_bBombDefused"_hash ) );
			bomb_site = memory::read<int>( planted_c4 + SCHEMA( "C_PlantedC4", "m_nBombSite"_hash ) );
			being_defused = memory::read<bool>( planted_c4 + SCHEMA( "C_PlantedC4", "m_bBeingDefused"_hash ) );
			timer_length = memory::read<float>( planted_c4 + SCHEMA( "C_PlantedC4", "m_flTimerLength"_hash ) );
		}

		if ( bomb_defused && !menu_open )
		{
			return;
		}

		const auto time_remaining = blow_time - current_time;
		const auto is_exploding = has_exploded || time_remaining <= 0.0f;

		if ( is_exploding && time_remaining < -2.0f && !menu_open )
		{
			return;
		}

		const auto calculate_bomb_damage = [ & ]( ) -> float
			{
				if ( !has_bomb )
				{
					return 0.0f;
				}

				const auto view_pawn = local.view_pawn( );
				if ( !view_pawn )
				{
					return 0.0f;
				}

				const auto c4_scene_node = memory::read<std::uintptr_t>( planted_c4 + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );
				const auto pawn_scene_node = memory::read<std::uintptr_t>( view_pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) );

				if ( !c4_scene_node || !pawn_scene_node )
				{
					return 0.0f;
				}

				const auto c4_origin = memory::read<math::vector3>( c4_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) );
				const auto pawn_origin = memory::read<math::vector3>( pawn_scene_node + SCHEMA( "CGameSceneNode", "m_vecAbsOrigin"_hash ) );

				const auto distance = ( c4_origin - pawn_origin ).length( );

				constexpr auto default_damage{ 650.0f };
				constexpr auto default_radius{ 2275.0f };

				const auto sigma = default_radius / 3.0f;
				auto damage = default_damage * std::exp( -( distance * distance ) / ( 2.0f * sigma * sigma ) );

				const auto armor = memory::read<int>( view_pawn + SCHEMA( "C_CSPlayerPawn", "m_ArmorValue"_hash ) );

				if ( armor > 0 )
				{
					constexpr auto armor_ratio = 0.5f;
					constexpr auto armor_bonus = 0.5f;

					auto armor_absorbed = damage * armor_ratio;
					auto armor_cost = ( damage - armor_absorbed ) * armor_bonus;

					if ( armor_cost > static_cast< float >( armor ) )
					{
						armor_cost = static_cast< float >( armor ) * ( 1.0f / armor_bonus );
						armor_absorbed = damage - armor_cost;
					}

					damage = armor_absorbed;
				}

				return std::floor( damage );
			}( );

		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;

		constexpr auto h{ 20.0f };
		constexpr auto top_offset{ 175.0f };
		constexpr auto r{ 0.0f };
		constexpr auto inner_r{ 0.0f };
		constexpr auto inner_pad{ 1.0f };
		constexpr auto text_pad_x{ 8.0f };
		constexpr auto text_nudge{ 0.5f };
		constexpr auto section_spacing{ 6.0f };

		const auto inner_h = h - inner_pad * 2.0f;

		auto timer_color = [ & ]( ) -> xdraw::color
			{
				if ( is_exploding )
				{
					return { 255, 100, 100, 255 };
				}

				const auto frac = timer_length > 0.0f ? time_remaining / timer_length : 1.0f;

				if ( frac > 0.5f )
				{
					return s.accent;
				}
				else if ( frac > 0.2f )
				{
					const auto t = ( frac - 0.2f ) / 0.3f;

					return
					{
						static_cast< std::uint8_t >( 255 ),
						static_cast< std::uint8_t >( 200 + static_cast< int >( ( s.accent.g - 200 ) * t ) ),
						static_cast< std::uint8_t >( 140 + static_cast< int >( ( s.accent.b - 140 ) * t ) ),
						255
					};
				}
				else
				{
					const auto t = frac / 0.2f;

					return
					{
						255,
						static_cast< std::uint8_t >( 120 + static_cast< int >( 80 * t ) ),
						static_cast< std::uint8_t >( 100 + static_cast< int >( 40 * t ) ),
						255
					};
				}
			}( );

		const auto site_label = bomb_site == 0 ? "A" : "B";
		const auto [site_tw, site_th] = xdraw::measure_text( site_label );
		const auto site_pill_w = site_tw + text_pad_x * 2.0f;

		const auto damage = static_cast< int >( calculate_bomb_damage );
		const auto view_pawn = local.view_pawn( );
		const auto health = view_pawn ? memory::read<int>( view_pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) ) : 0;
		const auto will_kill = health <= damage;

		const auto leftover = std::max( 0, health - damage );

		char health_buf[ 16 ]{};
		std::snprintf( health_buf, sizeof( health_buf ), "%d", leftover );

		const auto [health_vw, health_vh] = xdraw::measure_text( health_buf );
		const auto [health_uw, health_uh] = xdraw::measure_text( " health" );
		const auto health_pill_w = health_vw + health_uw + text_pad_x * 2.0f;
		const auto health_col = will_kill ? xdraw::color{ 255, 120, 120, 255 } : xdraw::color{ 160, 210, 140, 255 };

		char timer_buf[ 16 ]{};
		const char* timer_unit{};

		if ( is_exploding )
		{
			strncpy_s( timer_buf, sizeof( timer_buf ), "0.0s", _TRUNCATE );
			timer_unit = " exploding";
		}
		else
		{
			std::snprintf( timer_buf, sizeof( timer_buf ), "%.1fs", time_remaining );
			timer_unit = being_defused ? " defusing" : "";
		}

		const auto [timer_vw, timer_vh] = xdraw::measure_text( timer_buf );
		const auto [timer_uw, timer_uh] = xdraw::measure_text( timer_unit );
		const auto timer_pill_w = timer_vw + timer_uw + text_pad_x * 2.0f;

		const auto total_w = inner_pad + site_pill_w + section_spacing + health_pill_w + section_spacing + timer_pill_w + inner_pad;

		auto& bt_x = settings::g_esp.m_other.bombtimer_x.value;
		auto& bt_y = settings::g_esp.m_other.bombtimer_y.value;

		float x = bt_x >= 0.0f ? bt_x : ( static_cast< float >( screen_w ) - total_w ) * 0.5f;
		float y = bt_y >= 0.0f ? bt_y : top_offset;

		const auto& input = xui::ctx( ).input;
		const auto drag_rect = xui::rect{ x, y, total_w, h };

		static bool dragging{};
		static float grab_dx{}, grab_dy{};

		if ( !dragging && input.mouse_clicked && drag_rect.contains( input.mouse_x, input.mouse_y ) )
		{
			dragging = true;
			grab_dx = input.mouse_x - x;
			grab_dy = input.mouse_y - y;
		}

		if ( dragging )
		{
			if ( input.mouse_down )
			{
				x = std::clamp( input.mouse_x - grab_dx, 0.0f, static_cast< float >( screen_w ) - total_w );
				y = std::clamp( input.mouse_y - grab_dy, 0.0f, static_cast< float >( screen_h ) - h );

				bt_x = x;
				bt_y = y;
			}
			else
			{
				dragging = false;
			}
		}

		draw_list.rect_filled( x, y, total_w, h, s.window_bg, xdraw::corner_radius{ r } );
		draw_list.rect( x, y, total_w, h, tokens::col_border, xdraw::corner_radius{ r }, 1.0f );

		auto cx = x + inner_pad;

		draw_list.rect_filled( cx, y + inner_pad, site_pill_w, inner_h, s.child_bg, xdraw::corner_radius{ inner_r } );
		draw_list.text( cx + text_pad_x, y + ( h - site_th ) * 0.5f + text_nudge, site_label, s.accent );
		cx += site_pill_w + section_spacing;

		const auto health_unit_col = xdraw::color{ health_col.r, health_col.g, health_col.b, 120 };
		draw_list.rect_filled( cx, y + inner_pad, health_pill_w, inner_h, s.child_bg, xdraw::corner_radius{ inner_r } );
		draw_list.text( cx + text_pad_x, y + ( h - health_vh ) * 0.5f + text_nudge, health_buf, health_col );
		draw_list.text( cx + text_pad_x + health_vw, y + ( h - health_uh ) * 0.5f + text_nudge, " health", health_unit_col );
		cx += health_pill_w + section_spacing;

		const auto timer_unit_col = xdraw::color{ timer_color.r, timer_color.g, timer_color.b, 120 };
		draw_list.rect_filled( cx, y + inner_pad, timer_pill_w, inner_h, s.child_bg, xdraw::corner_radius{ inner_r } );
		draw_list.text( cx + text_pad_x, y + ( h - timer_vh ) * 0.5f + text_nudge, timer_buf, timer_color );
		draw_list.text( cx + text_pad_x + timer_vw, y + ( h - timer_uh ) * 0.5f + text_nudge, timer_unit, timer_unit_col );
	}

	void overlay::add_spectators( xdraw::draw_list& draw_list )
	{
		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || !systems::g_entities.exists( local.view_controller( ) ) )
		{
			return;
		}

		const auto game_rules = memory::read<std::uintptr_t>( addresses::globals::game_rules );
		if ( ( !game_rules || memory::read<int>( game_rules + SCHEMA( "C_CSGameRules", "m_gamePhase"_hash ) ) >= 4 ) && !rendering::g_menu.is_open( ) )
		{
			return;
		}

		const auto local_controller = local.controller;
		const auto view_controller = local.view_controller( );
		const auto view_pawn = local.view_pawn( );
		if ( !view_pawn )
		{
			return;
		}

		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;

		constexpr auto margin{ 10.0f };
		constexpr auto row_spacing{ 0.0f };
		constexpr auto row_h{ 18.0f };
		constexpr auto header_h{ hud::caption_h };
		constexpr auto pad_x{ 8.0f };
		constexpr auto text_nudge{ 0.5f };
		constexpr auto avatar_size{ 14.0f };

		struct spectator_entry
		{
			char name[ 128 ];
			std::uintptr_t steam_id;
		};

		spectator_entry entries[ 32 ]{};
		auto count{ 0 };

		for ( const auto& player : systems::g_entities.get_by_type( systems::entities::type::player ) )
		{
			if ( player.ptr == view_controller || player.ptr == local_controller || count >= 32 )
			{
				continue;
			}

			if ( memory::read<bool>( player.ptr + SCHEMA( "CCSPlayerController", "m_bPawnIsAlive"_hash ) ) )
			{
				continue;
			}

			const auto obs_pawn_handle = memory::read<std::uint32_t>( player.ptr + SCHEMA( "CCSPlayerController", "m_hObserverPawn"_hash ) );
			if ( !obs_pawn_handle || obs_pawn_handle == 0xffffffff )
			{
				continue;
			}

			const auto obs_pawn = systems::g_entities.lookup( obs_pawn_handle );
			if ( !obs_pawn )
			{
				continue;
			}

			const auto observer_services = memory::safe_read<std::uintptr_t>( obs_pawn + SCHEMA( "C_BasePlayerPawn", "m_pObserverServices"_hash ) ).value_or( 0 );
			if ( !observer_services || ( observer_services >> 48 ) != 0 )
			{
				continue;
			}

			const auto observer_target_handle = memory::safe_read<std::uint32_t>( observer_services + SCHEMA( "CPlayer_ObserverServices", "m_hObserverTarget"_hash ) ).value_or( 0 );
			if ( !observer_target_handle )
			{
				continue;
			}

			const auto observer_target = systems::g_entities.lookup( observer_target_handle );
			if ( observer_target != view_pawn )
			{
				continue;
			}

			const auto name_ptr = memory::read<std::uintptr_t>( player.ptr + SCHEMA( "CCSPlayerController", "m_sSanitizedPlayerName"_hash ) );
			if ( !name_ptr )
			{
				continue;
			}

			auto name = memory::read_string( name_ptr, 127 );
			std::ranges::transform( name, name.begin( ), [ ]( unsigned char c ) { return std::tolower( c ); } );

			auto& e = entries[ count++ ];
			strncpy_s( e.name, name.c_str( ), sizeof( e.name ) - 1 );

			e.name[ sizeof( e.name ) - 1 ] = '\0';
			e.steam_id = memory::read<std::uintptr_t>( player.ptr + SCHEMA( "CBasePlayerController", "m_steamID"_hash ) );
		}

		if ( count <= 0 && !rendering::g_menu.is_open( ) )
		{
			return;
		}

		static detail::avatar_cache avatars{};

		const auto [header_tw, header_th] = xdraw::measure_text( "spectators" );

		// One flat frame sized to the widest row, matching the keybind list.
		auto panel_w = header_tw + pad_x * 2.0f + hud::caption_gap * 2.0f + tokens::pad * 2.0f;
		for ( auto i = 0; i < count; ++i )
		{
			const auto [nw, nh] = xdraw::measure_text( entries[ i ].name );
			const auto has_avatar = avatars.get( entries[ i ].steam_id ) != nullptr;
			panel_w = std::max( panel_w, nw + pad_x * 2.0f + tokens::pad * 2.0f + ( has_avatar ? avatar_size + pad_x : 0.0f ) );
		}

		const auto panel_h = header_h + row_h * static_cast< float >( count );

		auto& sp_x = settings::g_esp.m_other.spectator_x.value;
		auto& sp_y = settings::g_esp.m_other.spectator_y.value;

		float x = sp_x >= 0.0f ? sp_x : std::max( margin, rendering::g_menu.pos_x( ) - panel_w - margin );
		float ry = sp_y >= 0.0f ? sp_y : std::max( margin, rendering::g_menu.pos_y( ) + tokens::window_h - panel_h - margin );

		const auto& input = xui::ctx( ).input;
		const auto header_rect = xui::rect{ x, ry, panel_w, std::min( header_h, panel_h ) };

		static bool dragging{};
		static float grab_dx{}, grab_dy{};

		if ( !dragging && input.mouse_clicked && header_rect.contains( input.mouse_x, input.mouse_y ) )
		{
			dragging = true;
			grab_dx = input.mouse_x - x;
			grab_dy = input.mouse_y - ry;
		}

		if ( dragging )
		{
			if ( input.mouse_down )
			{
				x = std::clamp( input.mouse_x - grab_dx, 0.0f, static_cast< float >( screen_w ) - panel_w );
				ry = std::clamp( input.mouse_y - grab_dy, 0.0f, static_cast< float >( screen_h ) - panel_h );

				sp_x = x;
				sp_y = ry;
			}
			else
			{
				dragging = false;
			}
		}

		if ( panel_h <= 0.0f )
		{
			return;
		}

		hud::list_frame( draw_list, x, ry, panel_w, panel_h, "spectators", 1.0f );

		ry += header_h;

		for ( auto i = 0; i < count; ++i )
		{
			const auto& e = entries[ i ];
			const auto [nw, nh] = xdraw::measure_text( e.name );
			const auto avatar_tex = avatars.get( e.steam_id );
			const auto has_avatar = avatar_tex != nullptr;
			const auto u8 = hud::fade_alpha( 1.0f );

			draw_list.rect_filled( x + 1.0f, ry + row_h - 1.0f, panel_w - 2.0f, 1.0f, tokens::col_border_soft.alpha( u8 ) );

			const auto text_x = x + tokens::pad + pad_x;
			draw_list.text( std::floor( text_x ), std::floor( ry + ( row_h - nh ) * 0.5f + text_nudge ), e.name, tokens::col_text.alpha( u8 ) );

			if ( has_avatar )
			{
				const auto ax = x + panel_w - tokens::pad - pad_x - avatar_size;
				const auto ay = ry + ( row_h - avatar_size ) * 0.5f;
				draw_list.image( std::floor( ax ), std::floor( ay ), avatar_size, avatar_size, avatar_tex, xdraw::corner_radius{ 0.0f } );
			}

			ry += row_h + row_spacing;
		}
	}

} // namespace features::esp::other
