#include <pch/pch.hpp>
#include <utilities/math/math.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

#include "../rendering.hpp"
#include <utilities/security/security.hpp>

namespace rendering {

	void widgets::draw( )
	{
		auto& dl = xdraw::get( );

		// In-game widgets render while the menu is closed, so re-apply the lime theme here.
		g_menu.sync_theme_style( );

		__try
		{
			this->watermark( dl );
			this->keybinds( dl );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	void widgets::watermark( xdraw::draw_list& draw_list )
	{
		const auto& cfg = settings::g_misc.m_watermark;

		static animation::fade container_alpha;
		if ( cfg.enabled.value )
			container_alpha.fade_in( 0.2f );
		else
			container_alpha.fade_out( 0.2f );

		container_alpha.update( );
		if ( !container_alpha.visible( ) )
			return;

		const auto alpha = container_alpha.alpha( );
		const auto u8 = hud::fade_alpha( alpha );
		const auto [screen_w, screen_h] = xdraw::viewport_size( );

		const auto scale = std::clamp( cfg.scale.value, 0.5f, 2.0f );
		const auto row_h = 15.0f * scale;
		const auto pad_x = 8.0f * scale;
		const auto pad = tokens::pad;

		// Build the stat segments first so the frame can be sized to them.
		struct segment
		{
			char text[ 32 ]{};
			bool warn{};
		};

		segment segments[ 5 ]{};
		auto count{ 0 };
		const auto push = [ & ]( const char* fmt, auto... args )
		{
			if ( count >= 5 )
				return;
			std::snprintf( segments[ count ].text, sizeof( segments[ count ].text ), fmt, args... );
			++count;
		};

		const auto local = systems::g_local.get( );
		const auto ping = local.controller
			? memory::safe_read<int>( local.controller + SCHEMA( "CCSPlayerController", "m_iPing"_hash ) ).value_or( 0 )
			: 0;
		const auto fps = static_cast< int >( std::round( xdraw::framerate( ) ) );
		const auto ping_warn = cfg.show_ping_warning.value && ping >= cfg.ping_warning.value;

		if ( cfg.show_name.value )
		{
			push( "%s", std::string( rendering::menu::k_brand_name ).c_str( ) );
		}
		if ( cfg.show_specs.value )
		{
			push( "specs %d", static_cast< int >( systems::g_entities.get_by_type( systems::entities::type::player ).size( ) ) );
		}
		if ( cfg.show_fps.value )
		{
			push( "fps %d", fps );
		}
		if ( cfg.show_ping.value )
		{
			push( "ping %d", ping );
			segments[ count - 1 ].warn = ping_warn;
		}
		if ( cfg.show_time.value )
		{
			SYSTEMTIME st{};
			GetLocalTime( &st );
			push( "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond );
		}

		if ( count == 0 )
			return;

		// Measure: fixed caption strip + hairline separators between segments.
		const auto [caption_tw, caption_th] = xdraw::measure_text( "watermark" );
		auto text_w = 0.0f;
		for ( auto i = 0; i < count; ++i )
		{
			const auto [w, h] = xdraw::measure_text( segments[ i ].text );
			text_w += w + ( i > 0 ? 7.0f * scale : 0.0f );
		}

		const auto content_w = text_w + pad_x * 2.0f;
		const auto panel_w = std::max( caption_tw + pad_x * 2.0f + hud::caption_gap * 2.0f + pad * 2.0f, content_w + pad * 2.0f );
		const auto panel_h = hud::caption_h + row_h;

		const auto margin = std::max( 0.0f, cfg.margin.value );
		const auto sw = static_cast< float >( screen_w );
		const auto sh = static_cast< float >( screen_h );

		float x = margin;
		float y = margin;

		switch ( cfg.anchor.value )
		{
		case settings::misc::watermark_anchor::top_right:
		case settings::misc::watermark_anchor::bottom_right:
			x = sw - panel_w - margin;
			break;
		case settings::misc::watermark_anchor::bottom_left:
			break;
		case settings::misc::watermark_anchor::top_center:
			x = ( sw - panel_w ) * 0.5f;
			break;
		case settings::misc::watermark_anchor::bottom_center:
			x = ( sw - panel_w ) * 0.5f;
			break;
		default:
			break;
		}

		switch ( cfg.anchor.value )
		{
		case settings::misc::watermark_anchor::bottom_left:
		case settings::misc::watermark_anchor::bottom_right:
		case settings::misc::watermark_anchor::bottom_center:
			y = sh - panel_h - margin;
			break;
		default:
			break;
		}

		hud::list_frame( draw_list, x, y, panel_w, panel_h, "watermark", alpha );

		// Segments on one row, separated by faint verticals instead of pills.
		const auto row_y = y + hud::caption_h;
		auto tx = x + pad + pad_x;
		const auto ty = row_y + ( row_h - caption_th ) * 0.5f;

		for ( auto i = 0; i < count; ++i )
		{
			if ( i > 0 )
			{
				draw_list.rect_filled( tx - 3.5f * scale, row_y + 3.0f * scale, 1.0f, row_h - 6.0f * scale, tokens::col_border.alpha( u8 ) );
				tx += 7.0f * scale;
			}

			const auto [w, h] = xdraw::measure_text( segments[ i ].text );
			const auto col = segments[ i ].warn ? tokens::col_accent : tokens::col_text_dim;
			draw_list.text( std::floor( tx ), std::floor( ty ), segments[ i ].text, col.alpha( u8 ) );
			tx += w;
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
		constexpr auto row_spacing{ 0.0f };
		constexpr auto row_h{ 18.0f };
		constexpr auto header_h{ hud::caption_h };
		constexpr auto pad_x{ 8.0f };
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

			auto is_legit_group{ false };
			for ( auto i = 0u; i < settings::combat::legitbot::k_group_count; ++i )
			{
				const auto& g = settings::g_combat.m_legitbot.groups[ i ];
				if ( setting == &g.aimbot || setting == &g.rcs || setting == &g.standalone_rcs || setting == &g.triggerbot || setting == &g.autowall || setting == &g.visualize_fov || setting == &g.trigger_always_on || setting == &g.trigger_autostop || setting == &g.visible_check || setting == &g.aim_through_smoke || setting == &g.aim_while_flashed || setting == &g.autoscope || setting == &g.silent_aim )
				{
					is_legit_group = true;
					break;
				}
			}

			if ( is_legit_group )
			{
				if ( !settings::g_combat.m_legitbot.enabled.value || !has_weapon )
				{
					continue;
				}

				const auto* active_group = &settings::g_combat.m_legitbot.get_group( ctx.weapon_type );
				auto is_active{ false };

				for ( auto i = 0u; i < settings::combat::legitbot::k_group_count; ++i )
				{
					if ( &settings::g_combat.m_legitbot.groups[ i ] == active_group )
					{
						const auto& g = settings::g_combat.m_legitbot.groups[ i ];
						if ( setting == &g.aimbot || setting == &g.rcs || setting == &g.standalone_rcs || setting == &g.triggerbot || setting == &g.autowall || setting == &g.visualize_fov || setting == &g.trigger_always_on || setting == &g.trigger_autostop || setting == &g.visible_check || setting == &g.aim_through_smoke || setting == &g.aim_while_flashed || setting == &g.autoscope || setting == &g.silent_aim )
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
				e.value[ 0 ] = '\0';
				e.has_value_pill = false;
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
		const auto header_w = header_tw + pad_x * 2.0f + hud::caption_gap * 2.0f + tokens::pad * 2.0f;

		// One flat frame sized to the widest row, so the list reads as a single
		// panel instead of a stack of separate floating pills.
		auto content_w = header_w;
		for ( auto i = 0; i < count; ++i )
		{
			const auto [nw, nh] = xdraw::measure_text( entries[ i ].name );
			auto row_text_w = nw;
			if ( entries[ i ].has_value_pill )
			{
				const auto [vw, vh] = xdraw::measure_text( entries[ i ].value );
				row_text_w += vw + pad_x * 2.0f;
			}
			content_w = std::max( content_w, row_text_w + pad_x * 2.0f + tokens::pad * 2.0f );
		}

		const auto panel_w = content_w;
		const auto panel_h = header_h + row_h * static_cast< float >( count );

		auto& kb_x = settings::g_misc.m_widgets.keybinds_x.value;
		auto& kb_y = settings::g_misc.m_widgets.keybinds_y.value;

		float x = kb_x >= 0.0f ? kb_x : std::max( margin, g_menu.pos_x( ) - panel_w - margin );
		float y = kb_y >= 0.0f ? kb_y : std::max( margin, g_menu.pos_y( ) + margin );

		const auto& input = xui::ctx( ).input;

		const auto header_rect = xui::rect{ x, y, panel_w, std::min( header_h, panel_h ) };

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

				kb_x = x;
				kb_y = y;
			}
			else
			{
				dragging = false;
			}
		}

		if ( panel_h > 0.0f )
		{
			hud::list_frame( draw_list, x, y, panel_w, panel_h, "keybinds", master_alpha );
		}

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
			const auto draw_y = y + anim.offset_y.value( );
			const auto [nw, nh] = xdraw::measure_text( e.name );
			const auto u8 = hud::fade_alpha( row_alpha );

			// Hairline separator only - the frame already carries the border.
			draw_list.rect_filled( x + 1.0f, draw_y + row_h - 1.0f, panel_w - 2.0f, 1.0f, tokens::col_border_soft.alpha( u8 ) );

			// Toggle binds are dim, hold/always binds carry the accent so the
			// list reads at a glance without a separate key column.
			const auto name_col = ( e.mode == xui::bind_mode::toggle ) ? tokens::col_text_dim : tokens::col_accent;
			draw_list.text( std::floor( x + tokens::pad + pad_x ), std::floor( draw_y + ( row_h - nh ) * 0.5f + text_nudge ), e.name, name_col.alpha( u8 ) );

			if ( e.has_value_pill )
			{
				const auto [vw, vh] = xdraw::measure_text( e.value );
				const auto vx = x + panel_w - tokens::pad - pad_x - vw;
				draw_list.text( std::floor( vx ), std::floor( draw_y + ( row_h - vh ) * 0.5f + text_nudge ), e.value, tokens::col_text.alpha( u8 ) );
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

				// Fading-out rows keep their own width in the old pill layout, so
				// they can stick out past the frame. Clip them to the panel instead.
				const auto row_alpha = it->second.alpha.alpha( ) * master_alpha;
				const auto draw_y = y + it->second.offset_y.value( );
				const auto u8 = hud::fade_alpha( row_alpha );

				draw_list.push_clip( x, y, panel_w, panel_h );
				draw_list.rect_filled( x + 1.0f, draw_y + row_h - 1.0f, panel_w - 2.0f, 1.0f, tokens::col_border_soft.alpha( u8 ) );

				const auto [nw, nh] = xdraw::measure_text( it->first.c_str( ) );
				draw_list.text( std::floor( x + tokens::pad + pad_x ), std::floor( draw_y + ( row_h - nh ) * 0.5f + text_nudge ), it->first, tokens::col_text_dim.alpha( u8 ) );
				draw_list.pop_clip( );
			}
			++it;
		}
	}

} // namespace rendering
