#include <pch/pch.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

#include "../../rendering.hpp"

namespace rendering {

	void menu::draw_lua( float group_w ) const
	{
		auto& dl = xui::draw::current( );
		const auto& s = xui::ctx( ).style;
		const auto& input = xui::ctx( ).input;

		const auto wx = this->m_x;
		const auto wy = this->m_y;

		xui::layout::set_cursor( this->m_body_x - wx, this->m_body_y - wy );

		if ( !xui::begin_child( "Lua##lua_panel", this->m_body_w, this->m_body_h, false ) )
		{
			return;
		}

		auto& lua = features::misc::g_lua;
		auto& cfg = settings::g_misc.m_lua;

		const auto header_y = xui::layout::get_cursor( ).second;

		xui::checkbox( "Enable lua scripts", cfg.enabled );

		if ( xui::button( "Reload", 90.0f, 26.0f ) )
		{
			lua.request_reload( );
		}

		const auto folder = "folder: " + lua.scripts_directory_name( );
		dl.text( 10.0f, header_y + 6.0f, folder.c_str( ), tokens::col_text_dim );

		const auto names = lua.script_names( );

		constexpr auto btn_h{ 28.0f };
		const auto avail = xui::layout::avail( );
		const auto list_h = std::max( 80.0f, avail.second - btn_h - s.item_spacing_y );

		if ( xui::begin_child( "##lua_list", avail.first, list_h, true ) )
		{
			const auto row_w = xui::layout::avail( ).first;
			constexpr auto row_h{ 28.0f };
			constexpr auto box_w{ 18.0f };

			for ( auto i = 0; i < static_cast< int >( names.size( ) ); ++i )
			{
				const auto& name = names[ i ];
				const auto enabled = lua.is_script_enabled( name );

				const auto row = xui::layout::item( row_w, row_h );
				const auto hovered = input.in_rect( row );

				if ( hovered )
				{
					dl.rect_filled( row.x, row.y, row.w, row.h,
						tokens::col_elevated.alpha( 120 ), xdraw::corner_radius{ 6.0f } );
				}

				if ( hovered && input.mouse_clicked && !xui::ctx( ).overlay_blocking( ) )
				{
					lua.toggle_script( name );
				}

				dl.rect_filled( row.x + 10.0f, row.y + ( row.h - box_w ) * 0.5f, box_w, box_w,
					enabled ? tokens::col_accent : tokens::col_elevated, xdraw::corner_radius{ 4.0f } );

				const auto text_size = xdraw::measure_text( name );
				dl.text( row.x + 10.0f + box_w + 8.0f, row.y + ( row.h - text_size.second ) * 0.5f, name,
					enabled ? tokens::col_text : tokens::col_text_dim );
			}

			if ( names.empty( ) )
			{
				const auto row = xui::layout::item( row_w, row_h );
				dl.text( row.x + 10.0f, row.y + 6.0f, "No .lua scripts found", tokens::col_text_dim );
			}

			xui::end_child( );
		}

		if ( !names.empty( ) )
		{
			if ( xui::button( "Enable all", 110.0f, btn_h ) )
			{
				lua.enable_all_scripts( );
			}

			xui::layout::same_line( );

			if ( xui::button( "Disable all", 110.0f, btn_h ) )
			{
				lua.disable_all_scripts( );
			}
		}

		const auto& error = lua.last_error( );
		if ( !error.empty( ) )
		{
			const auto error_y = xui::layout::get_cursor( ).second;
			dl.text( 10.0f, error_y + 6.0f, error.c_str( ), tokens::col_text_dim );
		}

		xui::end_child( );
	}

}