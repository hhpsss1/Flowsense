#include <pch/pch.hpp>
#include <core/settings.hpp>
#include <core/systems/systems.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		// Cham material combo with "Flat 2" hidden. The visible list maps back
		// onto settings::esp::cham_ids so the stored enum values stay aligned.
		inline static bool cham_material_combo( std::string_view label, settings::esp::cham_ids& value )
		{
			static constexpr const char* names[ ]{
				"Metallic", "Matte", "Flat", "Bloom", "Outlines", "Glow", "Glow 2",
				"Flow", "Dark Matter", "Data",
				"Metallic (iz)", "Matte (iz)", "Flat (iz)", "Bloom (iz)", "Outlines (iz)", "Glow (iz)", "Glow 2 (iz)",
				"Flow (iz)", "Dark Matter (iz)", "Data (iz)"
			};
			static constexpr auto count{ static_cast< int >( std::size( names ) ) };
			static constexpr std::array<int, count> map{ 0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21 };

			static std::unordered_map<std::uintptr_t, int> current{};

			const auto id = xui::make_id( label );
			auto& cur = current[ id ];

			if ( !xui::overlays::find( id ) )
			{
				cur = static_cast< int >( value );
			}

			if ( xui::combo( label, cur, names, count ) )
			{
				value = static_cast< settings::esp::cham_ids >( map[ static_cast< std::size_t >( cur ) ] );
				return true;
			}

			value = static_cast< settings::esp::cham_ids >( map[ static_cast< std::size_t >( std::clamp( cur, 0, count - 1 ) ) ] );
			return false;
		}

		// One row per chams config: an enable checkbox plus a "⋯" menu on the right.
		// The menu exposes every layer (primary/secondary/overlay) with a per-layer
		// enable checkbox, material combo and colour picker. Labels carry the config
		// suffix so each row gets its own xui widget state.
		inline static void draw_chams_config( const char* label, const char* id_suffix, settings::esp::chams_config& cfg )
		{
			xui::checkbox( label, cfg.enabled );

			char popup_id[ 64 ]{};
			std::snprintf( popup_id, sizeof( popup_id ), "##chams_%s", id_suffix );
			if ( xui::begin_popup( popup_id, 250.0f ) )
			{
				const auto layer_row = [ & ]( const char* name, settings::esp::chams_layer& layer )
					{
						char id_e[ 64 ]{}, id_m[ 64 ]{}, id_c[ 64 ]{}, id_ic[ 64 ]{};
						std::snprintf( id_e, sizeof( id_e ), "%s##%s_%s", name, name, id_suffix );
						std::snprintf( id_m, sizeof( id_m ), "Material##%s_%s", name, id_suffix );
						std::snprintf( id_c, sizeof( id_c ), "Color##%s_%s", name, id_suffix );
						std::snprintf( id_ic, sizeof( id_ic ), "Invisible color##%s_%s", name, id_suffix );

						xui::checkbox( id_e, layer.enabled );
						cham_material_combo( id_m, layer.material.value );
						xui::color_picker( id_c, layer.color );
						xui::color_picker( id_ic, layer.invisible_color );
					};

				layer_row( "Primary", cfg.primary );
				layer_row( "Secondary", cfg.secondary );
				layer_row( "Render", cfg.overlay );
				xui::end_popup( );
			}
		}

	} // namespace detail

	void menu::draw_player( float group_w ) const
	{
		auto& esp = settings::g_esp;
		auto& p = esp.m_player;

		const auto subtab = this->m_subtab;
		const auto has_overlay = ( subtab <= 1 );
		const auto col_w = has_overlay ? ( this->m_body_w - tokens::gap * 2.0f ) / 3.0f : ( this->m_body_w - tokens::gap ) * 0.5f;

		auto& glow = (subtab == 0) ? p.m_glow.enemy : p.m_glow.team;
		auto& glow_ragdoll = (subtab == 0) ? p.m_glow.enemy_ragdoll : p.m_glow.team_ragdoll;

		xui::layout::set_cursor( this->m_body_x - this->m_x, this->m_body_y - this->m_y );

		if ( has_overlay )
		{
			auto& ov = p.m_overlay[ subtab ];

			if ( xui::begin_child( "player esp", col_w ) )
			{
				xui::checkbox( "Esp overlay", ov.enabled );

				xui::checkbox( "Bounding box", ov.m_box.enabled );
				if ( xui::begin_popup( "##box_popup", 220.0f ) )
				{
					constexpr const char* box_styles[ ]{ "Full", "Cornered" };
					xui::combo( "Style##box", ov.m_box.style.value, box_styles, 2 );

					xui::checkbox( "Fill", ov.m_box.fill );
					xui::checkbox( "Outline", ov.m_box.outline );
					xui::slider_float( "Corner length", ov.m_box.corner_length, 2.0f, 20.0f, "%.0f" );
					xui::color_picker( "Visible color##box", ov.m_box.visible_color );
					xui::color_picker( "Occluded color##box", ov.m_box.occluded_color );
					xui::end_popup( );
				}

				xui::checkbox( "Skeleton", ov.m_skeleton.enabled );
				if ( xui::begin_popup( "##skeleton_popup", 220.0f ) )
				{
					xui::slider_float( "Thickness##skel", ov.m_skeleton.thickness, 0.5f, 4.0f, "%.1f" );
					xui::color_picker( "Visible color##skel", ov.m_skeleton.visible_color );
					xui::color_picker( "Occluded color##skel", ov.m_skeleton.occluded_color );
					xui::end_popup( );
				}

				xui::checkbox( "Health bar", ov.m_health_bar.enabled );
				if ( xui::begin_popup( "##health_popup", 220.0f ) )
				{
					constexpr const char* bar_positions[ ]{ "Left", "Top", "Bottom" };
					xui::combo( "Position##hp", ov.m_health_bar.position.value, bar_positions, 3 );

					xui::checkbox( "Outline##hp", ov.m_health_bar.outline_setting );
					xui::checkbox( "Gradient##hp", ov.m_health_bar.gradient );
					xui::checkbox( "Show value##hp", ov.m_health_bar.show_value );
					xui::checkbox( "Glow##hp", ov.m_health_bar.glow );
					xui::color_picker( "Full color##hp", ov.m_health_bar.full_color );
					xui::color_picker( "Low color##hp", ov.m_health_bar.low_color );
					xui::color_picker( "Background##hp", ov.m_health_bar.background_color );
					xui::color_picker( "Outline color##hp", ov.m_health_bar.outline_color );
					xui::color_picker( "Text color##hp", ov.m_health_bar.text_color );
					xui::color_picker( "Glow color##hp", ov.m_health_bar.glow_color );
					xui::slider_float( "Glow strength##hp", ov.m_health_bar.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::end_popup( );
				}

				xui::checkbox( "Ammo bar", ov.m_ammo_bar.enabled );
				if ( xui::begin_popup( "##ammo_popup", 220.0f ) )
				{
					constexpr const char* bar_positions[ ]{ "Left", "Top", "Bottom" };
					xui::combo( "Position##ammo", ov.m_ammo_bar.position.value, bar_positions, 3 );

					xui::checkbox( "Outline##ammo", ov.m_ammo_bar.outline_setting );
					xui::checkbox( "Gradient##ammo", ov.m_ammo_bar.gradient );
					xui::checkbox( "Show value##ammo", ov.m_ammo_bar.show_value );
					xui::checkbox( "Glow##ammo", ov.m_ammo_bar.glow );
					xui::color_picker( "Full color##ammo", ov.m_ammo_bar.full_color );
					xui::color_picker( "Low color##ammo", ov.m_ammo_bar.low_color );
					xui::color_picker( "Background##ammo", ov.m_ammo_bar.background_color );
					xui::color_picker( "Outline color##ammo", ov.m_ammo_bar.outline_color );
					xui::color_picker( "Text color##ammo", ov.m_ammo_bar.text_color );
					xui::color_picker( "Glow color##ammo", ov.m_ammo_bar.glow_color );
					xui::slider_float( "Glow strength##ammo", ov.m_ammo_bar.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::end_popup( );
				}

				xui::checkbox( "Name", ov.m_name.enabled );
				if ( xui::begin_popup( "##name_popup", 220.0f ) )
				{
					xui::color_picker( "Color##name", ov.m_name.color );
					xui::end_popup( );
				}

				xui::checkbox( "Weapon", ov.m_weapon.enabled );
				if ( xui::begin_popup( "##weapon_popup", 220.0f ) )
				{
					constexpr const char* display_types[ ]{ "Text", "Icon", "Text + icon" };
					xui::combo( "Display##wep", ov.m_weapon.display.value, display_types, 3 );

					xui::color_picker( "Text color##wep", ov.m_weapon.text_color );
					xui::color_picker( "Icon color##wep", ov.m_weapon.icon_color );
					xui::end_popup( );
				}

				xui::checkbox( "Info flags", ov.m_info_flags.enabled );
				if ( xui::begin_popup( "##flags_popup", 220.0f ) )
				{
					constexpr const char* flag_names[ ]{ "Money", "Armor", "Kit", "Scoped", "Defusing", "Flashed", "Ping", "Distance" };
					xui::multicombo( "Flags##mc", ov.m_info_flags.flags, flag_names, settings::esp::player::overlay::info_flags::count );

					xui::color_picker( "Money##flags", ov.m_info_flags.money_color );
					xui::color_picker( "Armor##flags", ov.m_info_flags.armor_color );
					xui::color_picker( "Kit##flags", ov.m_info_flags.kit_color );
					xui::color_picker( "Scoped##flags", ov.m_info_flags.scoped_color );
					xui::color_picker( "Defusing##flags", ov.m_info_flags.defusing_color );
					xui::color_picker( "Flashed##flags", ov.m_info_flags.flashed_color );
					xui::color_picker( "Distance##flags", ov.m_info_flags.distance_color );
					xui::end_popup( );
				}

				xui::checkbox( "Off screen arrow", ov.m_oof_arrow.enabled );
				if ( xui::begin_popup( "##oof_popup", 220.0f ) )
				{
					xui::checkbox( "Glow##oof", ov.m_oof_arrow.glow );
					xui::slider_float( "Width##oof", ov.m_oof_arrow.width, 4.0f, 40.0f, "%.0f" );
					xui::slider_float( "Height##oof", ov.m_oof_arrow.height, 4.0f, 40.0f, "%.0f" );
					xui::slider_float( "Radius x##oof", ov.m_oof_arrow.radius_x, 50.0f, 600.0f, "%.0f" );
					xui::slider_float( "Radius y##oof", ov.m_oof_arrow.radius_y, 50.0f, 600.0f, "%.0f" );
					xui::slider_float( "Glow strength##oof", ov.m_oof_arrow.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::color_picker( "Visible color##oof", ov.m_oof_arrow.visible_color );
					xui::color_picker( "Occluded color##oof", ov.m_oof_arrow.occluded_color );
					xui::end_popup( );
				}


				xui::end_child ();

				if (xui::begin_child ("##player_glow", col_w)) {
					xui::checkbox ("Glow", glow.enabled);
					if (xui::begin_popup ("##glow_popup", 220.0f)) {
						xui::color_picker ("Color##glow", glow.color);
						xui::end_popup ();
					}

					xui::checkbox ("Ragdoll", glow_ragdoll.enabled);
					if (xui::begin_popup ("##glow_rag_popup", 220.0f)) {
						xui::color_picker ("Color##glow_rag", glow_ragdoll.color);
						xui::end_popup ();
					}

					xui::end_child ();
				}


			}
		}
		else
		{
			if ( xui::begin_child( "chams & glow", col_w ) )
			{
				detail::draw_chams_config( "Chams", "local_main", p.m_chams.local );

				xui::layout::separator( );

				detail::draw_chams_config( "Ragdoll chams", "local_ragdoll", p.m_chams.local_ragdoll );

				xui::layout::separator( );

				xui::checkbox( "Glow", p.m_glow.local.enabled );
				if ( xui::begin_popup( "##local_glow_popup", 220.0f ) )
				{
					xui::color_picker( "Color##local_glow", p.m_glow.local.color );
					xui::end_popup( );
				}

				xui::checkbox( "Ragdoll", p.m_glow.local_ragdoll.enabled );
				if ( xui::begin_popup( "##local_glow_rag_popup", 220.0f ) )
				{
					xui::color_picker( "Color##local_glow_rag", p.m_glow.local_ragdoll.color );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}

		xui::layout::set_cursor( this->m_body_x - this->m_x + col_w + tokens::gap, this->m_body_y - this->m_y );

		if ( has_overlay )
		{
			auto& chams = ( subtab == 0 ) ? p.m_chams.enemy : p.m_chams.team;
			auto& chams_ragdoll = ( subtab == 0 ) ? p.m_chams.enemy_ragdoll : p.m_chams.team_ragdoll;

			if ( xui::begin_child( "player chams", col_w ) )
			{
				detail::draw_chams_config( "Chams", "main", chams );

				xui::layout::separator( );

				detail::draw_chams_config( "Ragdoll chams", "ragdoll", chams_ragdoll );

				if ( subtab == 0 )
				{
					xui::layout::separator( );

					// detail::draw_chams_config( "Backtrack chams", "bt", p.m_chams.backtrack );
					// detail::draw_chams_config( "Onshot", "os", p.m_chams.onshot );
				}

				xui::end_child( );
			}

			// Right column - reserved (model preview temporarily disabled)
			// systems::g_model_preview.draw_esp_preview( this->m_body_x + ( col_w + tokens::gap ) * 2.0f, this->m_body_y + this->m_scroll, col_w, this->m_body_h, subtab );
		}
		else
		{
			if ( xui::begin_child( "viewmodel", col_w ) )
			{
				detail::draw_chams_config( "Weapon chams", "vm_weapon", esp.m_viewmodel.weapon );

				xui::layout::separator( );

				detail::draw_chams_config( "Arms chams", "vm_arms", esp.m_viewmodel.arms );

				xui::end_child( );
			}
		}
	}

} // namespace rendering