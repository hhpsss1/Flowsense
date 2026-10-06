#include <pch/pch.hpp>
#include <core/resources/fonts/jetbrains_mono.hpp>
#include <utilities/math/math.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <map>
#include <utilities/steam/steam.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

#include "../rendering.hpp"
#include <utilities/security/security.hpp>

namespace rendering {

	void widgets::draw( )
	{
		auto& dl = xdraw::get( );

		g_menu.sync_theme_style( );

		__try
		{
			this->watermark( dl );
			this->indicators( dl );
			this->manual_aa( dl );
			this->keybinds( dl );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	void widgets::watermark( xdraw::draw_list& draw_list )
	{
		const auto& wm = settings::g_misc.m_watermark;
		if ( !wm.enabled.value )
		{
			return;
		}


		static auto* wm_font = xdraw::load_font( std::as_bytes( std::span{ resources::fonts::jetbrains_mono::regular } ), 13.5f );
		xdraw::push_font( wm_font );

		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;
		const auto& input = xui::ctx( ).input;

		const auto local = systems::g_local.get( );

		SYSTEMTIME st{};
		GetLocalTime( &st );
		char time_buf[ 16 ]{};
		std::snprintf( time_buf, sizeof( time_buf ), "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond );

		static auto smoothed_fps{ 0.0f };
		const auto framerate = xdraw::framerate( );
		if ( smoothed_fps == 0.0f )
		{
			smoothed_fps = framerate;
		}
		smoothed_fps += ( framerate - smoothed_fps ) * std::min( 3.0f * xdraw::delta_time( ), 1.0f );
		char fps_val[ 16 ]{};
		std::snprintf( fps_val, sizeof( fps_val ), "%d", static_cast< int >( std::round( smoothed_fps ) ) );

		auto ping{ 0 };
		if ( local.is_alive && local.controller && systems::g_entities.exists( local.controller ) )
		{
			ping = memory::read<std::uint32_t>( local.controller + SCHEMA( "CCSPlayerController", "m_iPing"_hash ) );
		}
		char ping_val[ 16 ]{};
		std::snprintf( ping_val, sizeof( ping_val ), "%dms", ping );

		const char* persona = steam::friends::get_persona_name( );
		const bool has_user = wm.show_user.value && persona && persona[ 0 ];
		const bool has_map = wm.show_map.value && !s_map_name.empty( );


		struct cell { const char* label; const char* value; bool show; bool accent; };
		const cell cells[ ]{
			{ "", "Flowsense", true, true },
			{ "", has_user ? persona : "", has_user, false },
			{ "FPS:", fps_val, wm.show_fps.value, false },
			{ "PING:", ping_val, wm.show_ping.value, false },
			{ "TIME:", time_buf, wm.show_time.value, false },
			{ "", s_map_name.c_str( ), has_map, false },
		};

		constexpr auto pad_x{ 6.0f };
		constexpr auto sep_w{ 8.0f };
		constexpr auto h{ 19.0f };

		auto w = pad_x * 2.0f;
		auto first = true;
		for ( const auto& c : cells )
		{
			if ( !c.show || ( !c.label[ 0 ] && !c.value[ 0 ] ) )
			{
				continue;
			}
			if ( !first )
			{
				w += sep_w;
			}
			if ( c.label[ 0 ] )
			{
				w += xdraw::measure_text( c.label ).first + 4.0f;
			}
			w += xdraw::measure_text( c.value ).first;
			first = false;
		}

		auto& wx = settings::g_misc.m_widgets.watermark_x.value;
		auto& wy = settings::g_misc.m_widgets.watermark_y.value;

		float x = wx >= 0.0f ? wx : static_cast< float >( screen_w ) - w - 10.0f;
		float y = wy >= 0.0f ? wy : 10.0f;

		static bool dragging{};
		static float grab_dx{}, grab_dy{};
		const auto header_rect = xui::rect{ x, y, w, h };
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
				x = std::clamp( input.mouse_x - grab_dx, 0.0f, static_cast< float >( screen_w ) - w );
				y = std::clamp( input.mouse_y - grab_dy, 0.0f, static_cast< float >( screen_h ) - h );
				wx = x;
				wy = y;
			}
			else
			{
				dragging = false;
			}
		}

		draw_list.rect_filled( x, y, w, h, s.window_bg, xdraw::corner_radius{ 4.0f } );
		draw_list.rect( x, y, w, h, s.child_border, xdraw::corner_radius{ 4.0f }, 1.0f );

		auto cx = x + pad_x;
		first = true;
		for ( const auto& c : cells )
		{
			if ( !c.show || ( !c.label[ 0 ] && !c.value[ 0 ] ) )
			{
				continue;
			}
			if ( !first )
			{
				const auto [sw, sh] = xdraw::measure_text( "|" );
				draw_list.text( cx, y + ( h - sh ) * 0.5f, "|", s.text_dim );
				cx += sep_w;
			}
			if ( c.label[ 0 ] )
			{
				const auto [lw2, lh2] = xdraw::measure_text( c.label );
				draw_list.text( cx, y + ( h - lh2 ) * 0.5f, c.label, s.text_dim );
				cx += lw2 + 4.0f;
			}
			const auto [tw, th] = xdraw::measure_text( c.value );
			draw_list.text( cx, y + ( h - th ) * 0.5f, c.value, c.accent ? s.accent : s.text );
			cx += tw;
			first = false;
		}

		xdraw::pop_font( );
	}

	void widgets::indicators( xdraw::draw_list& draw_list )
	{
		const auto& cfg = settings::g_misc.m_indicators;
		if ( !cfg.enabled.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || !local.is_alive )
		{
			return;
		}

		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;
		const auto& input = xui::ctx( ).input;

		struct entry
		{
			const char* key;
			std::string label{};
			std::string value{};
		};

		std::vector<entry> active{};

		const auto& ctx = features::combat::g_shared.ctx( );
		const auto has_weapon = ctx.valid && ctx.weapon_type >= cstypes::weapon_type::pistol && ctx.weapon_type <= cstypes::weapon_type::lmg;

		if ( settings::g_combat.m_ragebot.enabled.value && has_weapon )
		{
			const auto& group = settings::g_combat.m_ragebot.get_group( ctx.weapon_type );

			if ( cfg.show_body_aim.value && group.body_aim.value )
			{
				active.push_back( { "baim", "BAIM" } );
			}
			if ( cfg.show_min_damage.value && group.min_damage_override.value )
			{
				active.push_back( { "dmg", "DMG", std::to_string( group.min_damage_override_value.value ) } );
			}
			if ( cfg.show_hitchance.value && group.hitchance_override.value )
			{
				active.push_back( { "hc", "HC", std::format( "{}%", group.hitchance_override_value.value ) } );
			}
			if ( cfg.show_nospread.value && group.no_spread.value )
			{
				active.push_back( { "ns", "NO-SPREAD" } );
			}
		}

		if ( cfg.show_jumpbug.value && settings::g_movement.jumpbug.value )
		{
			active.push_back( { "jb", "JB" } );
		}
		if ( cfg.show_edgebug.value && settings::g_movement.edgebug.value )
		{
			active.push_back( { "eb", "EB" } );
		}
		if ( cfg.show_slowwalk.value && settings::g_movement.slowwalk.value )
		{
			active.push_back( { "slow", "SLOW" } );
		}

		struct pill_state
		{
			animation::fade alpha{};
			bool active_this_frame{};
			std::string label{};
			std::string value{};
		};

		static std::map<std::string, pill_state> states{};

		for ( auto& [key, state] : states )
		{
			state.active_this_frame = false;
		}

		for ( const auto& item : active )
		{
			auto& state = states[ item.key ];
			state.active_this_frame = true;
			state.label = item.label;
			state.value = item.value;
			state.alpha.fade_in( 0.18f );
			state.alpha.update( );
		}

		for ( auto it = states.begin( ); it != states.end( ); )
		{
			if ( !it->second.active_this_frame )
			{
				it->second.alpha.fade_out( 0.18f );
				it->second.alpha.update( );

				if ( it->second.alpha.alpha( ) <= 0.001f )
				{
					it = states.erase( it );
					continue;
				}
			}
			++it;
		}


		constexpr auto pill_h{ 20.0f };
		constexpr auto header_h{ 22.0f };
		constexpr auto pill_r{ 4.0f };
		constexpr auto text_pad_x{ 8.0f };
		constexpr auto spacing{ 3.0f };

		auto has_visible{ false };
		for ( const auto& [key, state] : states )
		{
			if ( state.alpha.alpha( ) > 0.01f )
			{
				has_visible = true;
				break;
			}
		}

		if ( !has_visible && !g_menu.is_open( ) )
		{
			return;
		}

		const auto [header_tw, header_th] = xdraw::measure_text( "indicators" );
		const auto header_w = text_pad_x * 2.0f + header_tw;

		auto& ix = settings::g_misc.m_widgets.indicators_x.value;
		auto& iy = settings::g_misc.m_widgets.indicators_y.value;

		float x = ix >= 0.0f ? ix : 16.0f;
		float base_y = iy >= 0.0f ? iy : static_cast< float >( screen_h ) * 0.5f + 25.0f;

		static bool dragging{};
		static float grab_dx{}, grab_dy{};
		const auto header_rect = xui::rect{ x, base_y, header_w, header_h };
		if ( !dragging && input.mouse_clicked && header_rect.contains( input.mouse_x, input.mouse_y ) )
		{
			dragging = true;
			grab_dx = input.mouse_x - x;
			grab_dy = input.mouse_y - base_y;
		}

		if ( dragging )
		{
			if ( input.mouse_down )
			{
				x = std::clamp( input.mouse_x - grab_dx, 0.0f, static_cast< float >( screen_w ) - header_w );
				base_y = std::clamp( input.mouse_y - grab_dy, 0.0f, static_cast< float >( screen_h ) - header_h );
				ix = x;
				iy = base_y;
			}
			else
			{
				dragging = false;
			}
		}

		draw_list.rect_filled( x, base_y, header_w, header_h, s.window_bg, xdraw::corner_radius{ pill_r } );
		draw_list.rect( x, base_y, header_w, header_h, s.child_border, xdraw::corner_radius{ pill_r }, 1.0f );
		draw_list.text( x + text_pad_x, base_y + ( header_h - header_th ) * 0.5f + 0.5f, "indicators", s.accent );

		auto offset_y = header_h + spacing;
		for ( const auto& [key, state] : states )
		{
			const auto alpha = state.alpha.alpha( );
			if ( alpha <= 0.01f )
			{
				continue;
			}

			const auto draw_y = base_y + offset_y;
			const auto [text_w, text_h] = xdraw::measure_text( state.label.c_str( ) );
			const auto [val_w, val_h] = state.value.empty( ) ? std::pair{ 0.0f, 0.0f } : xdraw::measure_text( state.value );
			const auto pill_w = text_pad_x * 2.0f + text_w + ( state.value.empty( ) ? 0.0f : 6.0f + val_w );

			auto bg = s.window_bg;
			bg.a = static_cast< std::uint8_t >( bg.a * alpha );
			auto border = s.child_border;
			border.a = static_cast< std::uint8_t >( border.a * alpha );
			auto tx_col = s.text;
			tx_col.a = static_cast< std::uint8_t >( tx_col.a * alpha );
			auto val_col = s.accent;
			val_col.a = static_cast< std::uint8_t >( val_col.a * alpha );

			draw_list.rect_filled( x, draw_y, pill_w, pill_h, bg, xdraw::corner_radius{ pill_r } );
			draw_list.rect( x, draw_y, pill_w, pill_h, border, xdraw::corner_radius{ pill_r }, 1.0f );
			draw_list.text( x + text_pad_x, draw_y + ( pill_h - text_h ) * 0.5f + 0.5f, state.label.c_str( ), tx_col );
			if ( !state.value.empty( ) )
			{
				draw_list.text( x + text_pad_x + text_w + 6.0f, draw_y + ( pill_h - val_h ) * 0.5f + 0.5f, state.value, val_col );
			}

			offset_y += pill_h + spacing;
		}
	}

	void widgets::manual_aa( xdraw::draw_list& draw_list )
	{
		const auto& cfg = settings::g_misc.m_indicators;
		if ( !cfg.show_manual_aa.value || !settings::g_combat.m_antiaim.enabled.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || !local.is_alive )
		{
			return;
		}

		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;
		const auto center_x = static_cast< float >( screen_w ) * 0.5f;
		const auto center_y = static_cast< float >( screen_h ) * 0.5f;
		constexpr auto distance{ 45.0f };

		static animation::fade left_anim{}, right_anim{};

		const auto& aa = settings::g_combat.m_antiaim;
		if ( aa.manual_left.value )
		{
			left_anim.fade_in( 0.18f );
		}
		else
		{
			left_anim.fade_out( 0.25f );
		}
		if ( aa.manual_right.value )
		{
			right_anim.fade_in( 0.18f );
		}
		else
		{
			right_anim.fade_out( 0.25f );
		}
		left_anim.update( );
		right_anim.update( );

		const auto blend = [ & ]( float t ) -> xdraw::color
			{
				const auto mix = [ t ]( std::uint8_t from, std::uint8_t to )
					{
						return static_cast< std::uint8_t >( static_cast< float >( from ) + ( static_cast< float >( to ) - static_cast< float >( from ) ) * t );
					};
				return xdraw::color{ mix( s.text_dim.r, s.accent.r ), mix( s.text_dim.g, s.accent.g ), mix( s.text_dim.b, s.accent.b ), mix( s.text_dim.a, s.accent.a ) };
			};

		if ( left_anim.alpha( ) > 0.01f )
		{
			const auto c = blend( left_anim.alpha( ) );
			draw_list.triangle_filled( center_x - distance - 8.0f, center_y, center_x - distance + 4.0f, center_y - 7.0f, center_x - distance + 4.0f, center_y + 7.0f, c );
		}
		if ( right_anim.alpha( ) > 0.01f )
		{
			const auto c = blend( right_anim.alpha( ) );
			draw_list.triangle_filled( center_x + distance + 8.0f, center_y, center_x + distance - 4.0f, center_y - 7.0f, center_x + distance - 4.0f, center_y + 7.0f, c );
		}
	}

	void widgets::keybinds( xdraw::draw_list& draw_list )
	{
		struct row_anim_t
		{
			animation::fade alpha;
			animation::spring offset_y;
			bool active_this_frame{ false };
		};

		static std::map<std::string, row_anim_t> row_states;
		static animation::fade container_alpha;
		const auto [screen_w, screen_h] = xdraw::viewport_size( );
		const auto& s = xui::ctx( ).style;

		constexpr auto margin{ 10.0f };
		constexpr auto row_spacing{ 3.0f };
		constexpr auto row_h{ 20.0f };
		constexpr auto header_h{ 22.0f };
		constexpr auto r{ 4.0f };
		constexpr auto text_pad_x{ 8.0f };
		constexpr auto text_nudge{ 0.5f };

		struct bind_entry
		{
			std::string name{};
			char value[ 32 ]{};
			bool has_value_pill{};
			xui::bind_mode mode{};
		};

		bind_entry entries[ 32 ]{};
		auto count{ 0 };

		const auto& ctx = features::combat::g_shared.ctx( );
		const auto has_weapon = ctx.valid && ctx.weapon_type >= cstypes::weapon_type::pistol && ctx.weapon_type <= cstypes::weapon_type::lmg;

		for ( const auto setting : xui::binds::all( ) )
		{
			if ( !setting || setting->name.empty( ) || setting->bind.key == 0 || !setting->bind.active || count >= 32 )
			{
				continue;
			}

			auto is_rage_group{ false };
			for ( auto i = 0u; i < settings::combat::ragebot::k_group_count; ++i )
			{
				const auto& g = settings::g_combat.m_ragebot.groups[ i ];
				if ( setting == &g.min_damage_override || setting == &g.hitchance_override || setting == &g.force_shot || setting == &g.force_shot_air || setting == &g.body_aim || setting == &g.silent || setting == &g.no_spread )
				{
					is_rage_group = true;
					break;
				}
			}

			if ( is_rage_group )
			{
				if ( !settings::g_combat.m_ragebot.enabled || !has_weapon )
				{
					continue;
				}

				const auto active_group = &settings::g_combat.m_ragebot.get_group( ctx.weapon_type );
				auto is_active{ false };

				for ( auto i = 0u; i < settings::combat::ragebot::k_group_count; ++i )
				{
					const auto& g = settings::g_combat.m_ragebot.groups[ i ];
					if ( &g == active_group )
					{
						if ( setting == &g.min_damage_override || setting == &g.hitchance_override || setting == &g.force_shot || setting == &g.force_shot_air || setting == &g.body_aim )
						{
							is_active = true;
						}
						break;
					}
				}

				if ( !is_active )
				{
					continue;
				}

				auto& e = entries[ count++ ];
				e.name = setting->name;
				e.mode = setting->bind.mode;

				if ( setting == &active_group->min_damage_override )
				{
					std::snprintf( e.value, sizeof( e.value ), "%d", active_group->min_damage_override_value.value );
					e.has_value_pill = true;
				}
				else if ( setting == &active_group->hitchance_override )
				{
					std::snprintf( e.value, sizeof( e.value ), "%d%%", active_group->hitchance_override_value.value );
					e.has_value_pill = true;
				}
				else
				{
					e.value[ 0 ] = '\0';
					e.has_value_pill = false;
				}
				continue;
			}

			if ( setting == &settings::g_combat.m_antiaim.enabled || setting == &settings::g_combat.m_antiaim.manual_left || setting == &settings::g_combat.m_antiaim.manual_right || setting == &settings::g_combat.m_antiaim.hide_shots || setting == &settings::g_combat.m_antiaim.avoid_backstab || setting == &settings::g_combat.m_antiaim.direction_indicator )
			{
				if ( !settings::g_combat.m_antiaim.enabled.value )
				{
					continue;
				}
			}

			auto& e = entries[ count++ ];
			e.name = setting->name;
			e.mode = setting->bind.mode;
			e.value[ 0 ] = '\0';
			e.has_value_pill = false;
		}

		if ( count > 0 || g_menu.is_open( ) )
			container_alpha.fade_in( 0.2f );
		else
			container_alpha.fade_out( 0.2f );

		container_alpha.update( );
		if ( !container_alpha.visible( ) )
			return;

		const auto master_alpha = container_alpha.alpha( );

		const auto [header_tw, header_th] = xdraw::measure_text( "keybinds" );
		const auto header_w = text_pad_x * 2.0f + header_tw;

		auto& kb_x = settings::g_misc.m_widgets.keybinds_x.value;
		auto& kb_y = settings::g_misc.m_widgets.keybinds_y.value;

		float x = kb_x >= 0.0f ? kb_x : std::max( margin, g_menu.pos_x( ) - header_w - margin );
		float y = kb_y >= 0.0f ? kb_y : std::max( margin, g_menu.pos_y( ) + margin );

		const auto& input = xui::ctx( ).input;

		const auto header_rect = xui::rect{ x, y, header_w, header_h };

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
				x = std::clamp( input.mouse_x - grab_dx, 0.0f, static_cast< float >( screen_w ) - header_w );
				y = std::clamp( input.mouse_y - grab_dy, 0.0f, static_cast< float >( screen_h ) - header_h );

				kb_x = x;
				kb_y = y;
			}
			else
			{
				dragging = false;
			}
		}

		const auto base_ry = y;

		draw_list.rect_filled( x, base_ry, header_w, header_h, s.window_bg.alpha( static_cast< std::uint8_t >( s.window_bg.a * master_alpha ) ), xdraw::corner_radius{ r } );
		draw_list.rect( x, base_ry, header_w, header_h, s.child_border.alpha( static_cast< std::uint8_t >( s.child_border.a * master_alpha ) ), xdraw::corner_radius{ r }, 1.0f );

		draw_list.text( x + text_pad_x, base_ry + ( header_h - header_th ) * 0.5f + text_nudge, "keybinds", s.accent.alpha( static_cast< std::uint8_t >( s.accent.a * master_alpha ) ) );

		for ( auto& [name, state] : row_states )
			state.active_this_frame = false;

		float current_offset_y = header_h + row_spacing;
		for ( auto i = 0; i < count; ++i )
		{
			const auto& e = entries[ i ];
			if ( e.name.empty( ) )
			{
				continue;
			}
			auto& anim = row_states[ e.name ];

			if ( !anim.active_this_frame && anim.alpha.alpha( ) <= 0.01f )
				anim.offset_y.snap( current_offset_y );

			anim.active_this_frame = true;
			anim.alpha.fade_in( 0.2f );
			anim.offset_y.set_target( current_offset_y );
			anim.alpha.update( );
			anim.offset_y.update( );

			const auto row_alpha = anim.alpha.alpha( ) * master_alpha;
			const auto draw_y = base_ry + anim.offset_y.value( );
			const auto [nw, nh] = xdraw::measure_text( e.name );

			if ( e.has_value_pill )
			{
				const auto [vw, vh] = xdraw::measure_text( e.value );
				const auto row_w = text_pad_x * 2.0f + nw + 8.0f + vw;

				draw_list.rect_filled( x, draw_y, row_w, row_h, s.window_bg.alpha( static_cast< std::uint8_t >( s.window_bg.a * row_alpha ) ), xdraw::corner_radius{ r } );
				draw_list.rect( x, draw_y, row_w, row_h, s.child_border.alpha( static_cast< std::uint8_t >( s.child_border.a * row_alpha ) ), xdraw::corner_radius{ r }, 1.0f );
				draw_list.text( x + text_pad_x, draw_y + ( row_h - nh ) * 0.5f + text_nudge, e.name, s.text.alpha( static_cast< std::uint8_t >( s.text.a * row_alpha ) ) );
				draw_list.text( x + row_w - text_pad_x - vw, draw_y + ( row_h - vh ) * 0.5f + text_nudge, e.value, s.accent.alpha( static_cast< std::uint8_t >( s.accent.a * row_alpha ) ) );
			}
			else
			{
				const auto row_w = text_pad_x * 2.0f + nw;

				draw_list.rect_filled( x, draw_y, row_w, row_h, s.window_bg.alpha( static_cast< std::uint8_t >( s.window_bg.a * row_alpha ) ), xdraw::corner_radius{ r } );
				draw_list.rect( x, draw_y, row_w, row_h, s.child_border.alpha( static_cast< std::uint8_t >( s.child_border.a * row_alpha ) ), xdraw::corner_radius{ r }, 1.0f );
				draw_list.text( x + text_pad_x, draw_y + ( row_h - nh ) * 0.5f + text_nudge, e.name, s.text.alpha( static_cast< std::uint8_t >( s.text.a * row_alpha ) ) );
			}

			current_offset_y += row_h + row_spacing;
		}

		for ( auto it = row_states.begin( ); it != row_states.end( ); )
		{
			if ( !it->second.active_this_frame )
			{
				it->second.alpha.fade_out( 0.15f );
				it->second.alpha.update( );
				it->second.offset_y.update( );

				if ( it->second.alpha.alpha( ) <= 0.001f )
				{
					it = row_states.erase( it );
					continue;
				}

				const auto row_alpha = it->second.alpha.alpha( ) * master_alpha;
				const auto draw_y = base_ry + it->second.offset_y.value( );
				const auto [nw, nh] = xdraw::measure_text( it->first.c_str( ) );
				const auto row_w = text_pad_x * 2.0f + nw;

				draw_list.rect_filled( x, draw_y, row_w, row_h, s.window_bg.alpha( static_cast< std::uint8_t >( s.window_bg.a * row_alpha ) ), xdraw::corner_radius{ r } );
				draw_list.rect( x, draw_y, row_w, row_h, s.child_border.alpha( static_cast< std::uint8_t >( s.child_border.a * row_alpha ) ), xdraw::corner_radius{ r }, 1.0f );
			}
			++it;
		}
	}

}
