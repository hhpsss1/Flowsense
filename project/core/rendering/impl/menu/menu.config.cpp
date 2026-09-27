#include <pch/pch.hpp>
#include <utilities/math/math.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		std::string name_buf{};
		std::vector<std::wstring> config_list{};
		auto selected{ -1 };
		auto needs_refresh{ true };
		auto confirm_delete{ false };
		auto confirm_timer{ 0.0f };

		static inline void wide_to_utf8( const std::wstring& wide, char* out, int out_size )
		{
			WideCharToMultiByte( CP_UTF8, 0, wide.c_str( ), -1, out, out_size, nullptr, nullptr );
		}

		static inline std::wstring utf8_to_wide( const std::string& utf8 )
		{
			wchar_t buf[ 128 ]{};
			MultiByteToWideChar( CP_UTF8, 0, utf8.c_str( ), -1, buf, 128 );
			return buf;
		}

		static inline std::string selected_name( )
		{
			if ( detail::selected < 0 || detail::selected >= static_cast< int >( detail::config_list.size( ) ) )
			{
				return {};
			}

			char narrow[ 128 ]{};
			wide_to_utf8( detail::config_list[ detail::selected ], narrow, sizeof( narrow ) );
			return narrow;
		}

	} // namespace detail

	void menu::draw_config( float group_w )
	{
		( void )group_w;

		// Re-scan C:\nexoria periodically while the tab is open so configs
		// dropped in externally show up without needing a create/save/delete.
		static float scan_timer{ 0.0f };
		scan_timer += xdraw::delta_time( );
		if ( scan_timer >= 1.0f )
		{
			scan_timer = 0.0f;
			detail::needs_refresh = true;
		}

		if ( detail::needs_refresh )
		{
			detail::config_list = config::registry::list( );
			detail::needs_refresh = false;

			if ( detail::selected >= static_cast< int >( detail::config_list.size( ) ) )
			{
				detail::selected = -1;
			}
		}

		const auto dt = xdraw::delta_time( );

		if ( detail::confirm_delete )
		{
			detail::confirm_timer += dt;

			if ( detail::confirm_timer > 3.0f )
			{
				detail::confirm_delete = false;
			}
		}

		auto& dl = xui::draw::current( );
		const auto& s = xui::ctx( ).style;
		const auto& input = xui::ctx( ).input;

		if ( this->m_subtab == 1 )
		{
			auto& m = settings::g_misc;

			xui::layout::set_cursor( this->m_body_x - this->m_x, this->m_body_y - this->m_y );

			if ( xui::begin_child( "config settings", this->m_body_w, this->m_body_h, false ) )
			{
				xui::checkbox( "Safe mode", m.safe_mode );
				if ( xui::begin_popup( "##safemode_popup", 220.0f ) )
				{
					xui::text( "Disables ragebot, anti-aim, and other risky features.", xdraw::color{ 180, 180, 180, 255 } );
					xui::end_popup( );
				}

				xui::layout::separator( );

				xui::keybind( "Menu key", m.menu_key );
				xui::slider_float( "Menu dpi scale", m.menu_dpi_scale, 0.5f, 2.0f, "%.2fx" );

				xui::end_child( );
			}

			return;
		}

		xui::layout::set_cursor( this->m_body_x - this->m_x, this->m_body_y - this->m_y );

		if ( !xui::begin_child( "cloud sync", this->m_body_w, this->m_body_h, false ) )
		{
			return;
		}

		xui::text_input( "##cfg_name", detail::name_buf, 64, "Config name..." );

		constexpr auto btn_h{ 28.0f };
		const auto [ avail_w, avail_h ] = xui::layout::avail( );
		const auto list_h = std::max( 80.0f, avail_h - btn_h - s.item_spacing_y );

		if ( xui::begin_child( "##cfg_list", avail_w, list_h, true ) )
		{
			const auto row_w = xui::layout::avail( ).first;
			constexpr auto row_h{ 28.0f };
			auto visible_rows{ 0 };

			for ( auto i = 0; i < static_cast< int >( detail::config_list.size( ) ); ++i )
			{
				const auto& wname = detail::config_list[ i ];

				char narrow[ 128 ]{};
				detail::wide_to_utf8( wname, narrow, sizeof( narrow ) );

				const auto row = xui::layout::item( row_w, row_h );
				const auto is_selected = ( detail::selected == i );
				const auto is_hovered = input.in_rect( row );

				if ( is_hovered && input.mouse_clicked && !xui::ctx( ).overlay_blocking( ) )
				{
					detail::selected = i;
					detail::confirm_delete = false;

					char sel_narrow[ 128 ]{};
					detail::wide_to_utf8( wname, sel_narrow, sizeof( sel_narrow ) );
					detail::name_buf = sel_narrow;
				}

				const auto hover_anim = xui::anim::lerp( xui::fnv1a( "cfgrow" ) + i, is_hovered ? 1.0f : 0.0f, 14.0f );
				const auto sel_anim = xui::anim::lerp( xui::fnv1a( "cfgsel" ) + i, is_selected ? 1.0f : 0.0f, 10.0f );

				if ( sel_anim > 0.01f )
				{
					dl.rect_filled( row.x, row.y, row.w, row.h, tokens::col_accent.alpha( static_cast< std::uint8_t >( 70.0f * sel_anim ) ) );
				}
				else if ( hover_anim > 0.01f )
				{
					dl.rect_filled( row.x, row.y, row.w, row.h, tokens::col_elevated.alpha( static_cast< std::uint8_t >( 255.0f * hover_anim * 0.5f ) ) );
				}

				if ( sel_anim > 0.01f )
				{
					// Accent bar in the leading gutter marks the active config.
					const auto bar_a = static_cast< std::uint8_t >( 255.0f * sel_anim );
					const auto bar_h = std::max( 0.0f, row.h - 8.0f );
					dl.rect_filled( row.x + 2.0f, row.y + 4.0f, 2.0f, bar_h, tokens::col_accent.alpha( bar_a ) );
				}

				const auto [ tw, th ] = xdraw::measure_text( narrow );
				const auto text_col = is_selected
					? xui::lerp( tokens::col_text, tokens::col_accent, sel_anim )
					: xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );

				dl.text( std::floor( row.x + 10.0f ), std::floor( row.y + ( row.h - th ) * 0.5f ), narrow, text_col );

				visible_rows++;
			}

			if ( visible_rows == 0 )
			{
				const auto row = xui::layout::item( row_w, row_h );
				dl.text( row.x + 10.0f, row.y + 6.0f, detail::config_list.empty( ) ? "No configs found" : "No matches", tokens::col_text_dim );
			}

			xui::end_child( );
		}

		const auto btn_w = ( avail_w - s.item_spacing_x * 4.0f ) / 5.0f;
		const auto has_selection = detail::selected >= 0 && detail::selected < static_cast< int >( detail::config_list.size( ) );
		const auto save_name = !detail::name_buf.empty( ) ? detail::name_buf : ( has_selection ? detail::selected_name( ) : "" );
		const auto can_save = !save_name.empty( );

		if ( xui::button( "Refresh", btn_w, btn_h ) )
		{
			detail::needs_refresh = true;
		}

		xui::layout::same_line( );

		if ( xui::button( "Load", btn_w, btn_h ) && has_selection )
		{
			config::registry::load( detail::config_list[ detail::selected ] );
			settings::finalize_binds( );
		}

		xui::layout::same_line( );

		if ( xui::button( "Create", btn_w, btn_h ) && !detail::name_buf.empty( ) )
		{
			const auto wname = detail::utf8_to_wide( detail::name_buf );
			config::registry::save( wname );
			detail::needs_refresh = true;
			detail::config_list = config::registry::list( );
			detail::needs_refresh = false;

			const auto it = std::find( detail::config_list.begin( ), detail::config_list.end( ), wname );
			detail::selected = ( it != detail::config_list.end( ) )
				? static_cast< int >( std::distance( detail::config_list.begin( ), it ) )
				: -1;
		}

		xui::layout::same_line( );

		if ( xui::button( "Save", btn_w, btn_h ) && can_save )
		{
			config::registry::save( detail::utf8_to_wide( save_name ) );
			detail::needs_refresh = true;
		}

		xui::layout::same_line( );

		if ( detail::confirm_delete )
		{
			xui::push_style_color( xui::style_col::button_border, tokens::col_accent );
			xui::push_style_color( xui::style_col::text, tokens::col_accent );

			if ( xui::button( "Confirm", btn_w, btn_h ) && has_selection )
			{
				config::registry::remove( detail::config_list[ detail::selected ] );
				detail::selected = -1;
				detail::needs_refresh = true;
				detail::confirm_delete = false;
			}

			xui::pop_style_color( 2 );
		}
		else if ( xui::button( "Delete", btn_w, btn_h ) && has_selection )
		{
			detail::confirm_delete = true;
			detail::confirm_timer = 0.0f;
		}

		xui::end_child( );
	}

} // namespace rendering
