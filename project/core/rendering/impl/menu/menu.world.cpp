#include <pch/pch.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

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

	} // namespace detail

	void menu::draw_world( float group_w ) const
	{
		auto& w = settings::g_world;

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		const auto content_x = this->m_content_x;
		const auto body_y = this->m_body_y;
		const auto content_w = this->m_content_w;
		const auto col_w = this->m_col_w;
		const auto right_x = content_x + col_w + tokens::gap;

		const auto subtab = this->m_subtab;

		xui::layout::set_cursor( content_x - wx, body_y - wy );

		if ( subtab == 0 )
		{
			auto& item = settings::g_esp.m_item;
			auto& proj = settings::g_esp.m_projectile;
			auto& other = settings::g_esp.m_other;

			constexpr const char* display_types[ ]{ "Text", "Icon", "Text + icon" };

			auto draw_chams_layer = [ & ]( const char* label, const char* popup_id, settings::esp::chams_layer& layer )
				{
					xui::checkbox( label, layer.enabled );
					if ( xui::begin_popup( popup_id, 220.0f ) )
					{
						detail::cham_material_combo( "Material", layer.material.value );
						xui::color_picker( "Color", layer.color );
						xui::color_picker( "Invisible color", layer.invisible_color );
						xui::end_popup( );
					}
				};

			xui::layout::set_cursor( content_x - wx, body_y - wy );

			if ( xui::begin_child( "world items", col_w ) )
			{
				static int item_group{};
				xui::combo( "Group##item_sel", item_group, settings::esp::item::k_group_names, settings::esp::item::k_group_count );

				xui::layout::separator( );

				xui::checkbox( "Item esp", item.m_overlay.group_toggle( item_group ) );
				if ( xui::begin_popup( "##ie_grp_cfg", 220.0f ) )
				{
					auto& g = item.m_overlay.groups[ item_group ];

					char id_d[ 32 ]{}, id_m[ 32 ]{}, id_t[ 32 ]{}, id_i[ 32 ]{};
					std::snprintf( id_d, sizeof( id_d ), "Display##ie%d", item_group );
					std::snprintf( id_m, sizeof( id_m ), "Max dist##ie%d", item_group );
					std::snprintf( id_t, sizeof( id_t ), "Text color##ie%d", item_group );
					std::snprintf( id_i, sizeof( id_i ), "Icon color##ie%d", item_group );

					xui::combo( id_d, g.display.value, display_types, 3 );
					xui::slider_float( id_m, g.max_distance, 1.0f, 200.0f, "%.0fm" );
					xui::color_picker( id_t, g.text_color );
					xui::color_picker( id_i, g.icon_color );
					xui::end_popup( );
				}

				xui::checkbox( "Item chams", item.m_chams.group_toggle( item_group ) );
				if ( xui::begin_popup( "##ic_grp_cfg", 220.0f ) )
				{
					auto& g = item.m_chams.groups[ item_group ];

					char id_p[ 48 ]{}, id_pp[ 48 ]{}, id_s[ 48 ]{}, id_sp[ 48 ]{};
					std::snprintf( id_p, sizeof( id_p ), "Primary##ic%d", item_group );
					std::snprintf( id_pp, sizeof( id_pp ), "##ic_p%d", item_group );
					std::snprintf( id_s, sizeof( id_s ), "Secondary##ic%d", item_group );
					std::snprintf( id_sp, sizeof( id_sp ), "##ic_s%d", item_group );

					draw_chams_layer( id_p, id_pp, g.primary );
					draw_chams_layer( id_s, id_sp, g.secondary );
					xui::end_popup( );
				}

				xui::checkbox( "Item glow", item.m_glow.group_toggle( item_group ) );
				if ( xui::begin_popup( "##ig_grp_cfg", 220.0f ) )
				{
					char id[ 32 ]{};
					std::snprintf( id, sizeof( id ), "Color##ig%d", item_group );
					xui::color_picker( id, item.m_glow.groups[ item_group ].color );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "projectiles", col_w ) )
			{
				static auto proj_group{ 0 };
				xui::combo( "Group##proj_sel", proj_group, settings::esp::projectile::k_group_names, settings::esp::projectile::k_group_count );

				xui::layout::separator( );

				const auto is_inferno = ( proj_group == 5 );

				xui::checkbox( is_inferno ? "Inferno esp" : "Projectile esp", proj.m_overlay.group_toggle( proj_group ) );

				if ( !is_inferno )
				{
					if ( xui::begin_popup( "##pe_grp_cfg", 220.0f ) )
					{
						auto& g = proj.m_overlay.groups[ proj_group ];

						char id_d[ 32 ]{}, id_m[ 32 ]{}, id_t[ 32 ]{}, id_i[ 32 ]{};
						std::snprintf( id_d, sizeof( id_d ), "Display##pe%d", proj_group );
						std::snprintf( id_m, sizeof( id_m ), "Max dist##pe%d", proj_group );
						std::snprintf( id_t, sizeof( id_t ), "Text color##pe%d", proj_group );
						std::snprintf( id_i, sizeof( id_i ), "Icon color##pe%d", proj_group );

						xui::combo( id_d, g.display.value, display_types, 3 );
						xui::slider_float( id_m, g.max_distance, 1.0f, 200.0f, "%.0fm" );
						xui::color_picker( id_t, g.text_color );
						xui::color_picker( id_i, g.icon_color );
						xui::end_popup( );
					}
				}
				else
				{
					if ( xui::begin_popup( "##inferno_cfg", 220.0f ) )
					{
						xui::color_picker( "Fill color##inf", proj.m_overlay.m_infernos.fill_color );
						xui::color_picker( "Outline color##inf", proj.m_overlay.m_infernos.outline_color );
						xui::slider_float( "Outline thickness##inf", proj.m_overlay.m_infernos.outline_thickness, 0.5f, 5.0f, "%.1f" );
						xui::checkbox( "Glow##inf", proj.m_overlay.m_infernos.glow );
						xui::slider_float( "Glow strength##inf", proj.m_overlay.m_infernos.glow_strength, 0.1f, 1.0f, "%.2f" );
						xui::end_popup( );
					}
				}

				const auto indicator_id = proj_group == 0 ? 0 : proj_group == 3 ? 1 : proj_group == 5 ? 2 : -1;
				if ( indicator_id >= 0 )
				{
					xui::checkbox( is_inferno ? "Indicator" : "Landing indicator", proj.m_overlay.m_indicator.get_group( indicator_id ).enabled );
					if ( xui::begin_popup( "##ind_grp_cfg", 220.0f ) )
					{
						auto& g = proj.m_overlay.m_indicator.get_group( indicator_id );

						char id_a[ 32 ]{}, id_i[ 32 ]{}, id_b[ 32 ]{}, id_g[ 32 ]{}, id_gs[ 32 ]{}, id_d[ 32 ]{}, id_t[ 32 ]{};
						std::snprintf( id_a, sizeof( id_a ), "Arc color##ind%d", indicator_id );
						std::snprintf( id_i, sizeof( id_i ), "Icon color##ind%d", indicator_id );
						std::snprintf( id_b, sizeof( id_b ), "Background##ind%d", indicator_id );
						std::snprintf( id_g, sizeof( id_g ), "Glow##ind%d", indicator_id );
						std::snprintf( id_gs, sizeof( id_gs ), "Glow strength##ind%d", indicator_id );
						std::snprintf( id_d, sizeof( id_d ), "Damage text##ind%d", indicator_id );
						std::snprintf( id_t, sizeof( id_t ), "Damage text color##ind%d", indicator_id );

						xui::color_picker( id_a, g.arc_color );
						xui::color_picker( id_i, g.icon_color );
						xui::color_picker( id_b, g.background_color );
						xui::checkbox( id_g, g.glow );
						xui::slider_float( id_gs, g.glow_strength, 0.1f, 1.0f, "%.2f" );
						xui::checkbox( id_d, g.show_damage );
						xui::color_picker( id_t, g.text_color );
						xui::end_popup( );
					}
				}

				xui::end_child( );
			}

			if ( xui::begin_child( "world overlays", col_w ) )
			{
				xui::checkbox( "Bomb timer", other.bomb_timer );
				xui::checkbox( "Spectator list", other.spectator_list );

				xui::end_child( );
			}
		}

		if ( subtab == 1 )
		{
			auto& scene = w.m_scene;
			auto& weather = w.m_weather;

			if ( xui::begin_child( "scene", col_w ) )
			{
				xui::checkbox( "Skybox material", scene.skybox.custom_skybox );
				if ( xui::begin_popup( "##skybox_popup", 220.0f ) )
				{
					const auto& skyboxes = features::world::g_scene.get_skyboxes( );
					if ( !skyboxes.empty( ) )
					{
						std::vector<const char*> names;
						names.reserve( skyboxes.size( ) );
						for ( const auto& skybox : skyboxes )
						{
							names.push_back( skybox.display_name.c_str( ) );
						}

						scene.skybox.selected_skybox.value = std::clamp(
							scene.skybox.selected_skybox.value, 0,
							static_cast<int>( skyboxes.size( ) ) - 1 );
						xui::combo(
							"Skybox", scene.skybox.selected_skybox.value,
							names.data( ), static_cast<int>( names.size( ) ) );
					}
					xui::end_popup( );
				}

				xui::checkbox( "Skybox color", scene.skybox.custom_color );
				if ( xui::begin_popup( "##skycolor_popup", 220.0f ) )
				{
					xui::color_picker( "Sky color", scene.skybox.skybox_color );
					xui::color_picker( "Cloud color", scene.skybox.cloud_color );
					xui::color_picker( "Sun color", scene.skybox.sun_color );
					xui::end_popup( );
				}

				xui::checkbox( "World color", scene.world_setting );
				if ( xui::begin_popup( "##worldcolor_popup", 220.0f ) )
				{
					xui::color_picker( "Color##world", scene.world_color );
					xui::end_popup( );
				}

				xui::checkbox( "Lighting", scene.lighting );
				if ( xui::begin_popup( "##lighting_popup", 220.0f ) )
				{
					xui::slider_float( "Intensity##light", scene.lighting_intensity, 0.0f, 2.0f, "%.2f" );
					xui::color_picker( "Color##light", scene.lighting_color );
					xui::end_popup( );
				}

				xui::checkbox( "Bloom", scene.bloom );
				if ( xui::begin_popup( "##bloom_popup", 220.0f ) )
				{
					xui::slider_float( "Value##bloom", scene.bloom_value, 0.0f, 2.0f, "%.2f" );
					xui::end_popup( );
				}

				xui::checkbox( "Gamma", scene.gamma );
				if ( xui::begin_popup( "##gamma_popup", 220.0f ) )
				{
					xui::slider_float( "Value##gamma", scene.gamma_value, 0.5f, 5.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::layout::separator( );

				xui::checkbox( "Weather", weather.enabled );
				if ( xui::begin_popup( "##weather_popup", 220.0f ) )
				{
					constexpr const char* weather_types[ ]{ "Snow", "Rain", "Stars" };
					xui::combo( "Type", weather.type.value, weather_types, 3 );

					xui::slider_float( "Intensity##weather", weather.intensity, 0.1f, 2.0f, "%.2f" );
					xui::color_picker( "Color##weather", weather.color );
					xui::end_popup( );
				}

				xui::checkbox( "Fog", weather.fog_enabled );
				if ( xui::begin_popup( "##fog_popup", 220.0f ) )
				{
					xui::slider_float( "Density##fog", weather.fog_density, 0.0f, 1.0f, "%.2f" );
					xui::slider_float( "Anisotropy", weather.fog_anisotropy, 0.0f, 1.0f, "%.2f" );
					xui::slider_float( "Draw distance", weather.fog_draw_distance, 500.0f, 20000.0f, "%.0f" );
					xui::color_picker( "Color##fog", weather.fog_color );
					xui::end_popup( );
				}

				xui::checkbox( "Rain", weather.wetness );
				if ( xui::begin_popup( "##wetness_popup", 220.0f ) )
				{
					xui::slider_float( "Density##wet", weather.wetness_density, 0.0f, 5.0f, "%.1f" );
					xui::slider_float( "Speed##wet", weather.wetness_speed, 0.0f, 3.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::checkbox( "Wind", weather.wind );
				if ( xui::begin_popup( "##wind_popup", 220.0f ) )
				{
					xui::slider_float( "Strength##wind", weather.wind_strength, 0.0f, 5.0f, "%.1f" );
					xui::slider_float( "Direction##wind", weather.wind_direction, 0.0f, 360.0f, "%.0f" );
					xui::slider_float( "Turbulence##wind", weather.wind_turbulence, 0.0f, 5.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "scene lighting", col_w ) )
			{
				xui::checkbox( "Depth of field", scene.dof );
				if ( xui::begin_popup( "##dof_popup", 220.0f ) )
				{
					xui::slider_float( "Near blurry", scene.dof_near_blurry, 0.0f, 50.0f, "%.0f" );
					xui::slider_float( "Near crisp", scene.dof_near_crisp, 0.0f, 100.0f, "%.0f" );
					xui::slider_float( "Far crisp", scene.dof_far_crisp, 100.0f, 2000.0f, "%.0f" );
					xui::slider_float( "Far blurry", scene.dof_far_blurry, 200.0f, 5000.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}
	}

} // namespace rendering
