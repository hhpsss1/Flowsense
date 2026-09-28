#include <pch/pch.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		constexpr const char* sound_types[ ]{ "Shop click", "Home click", "Bell", "Killcard", "Bullet casing", "Coin pickup", "Item drop", "Popcan", "Key press", "Custom" };
		constexpr auto k_sound_type_count{ static_cast< int >( std::size( sound_types ) ) };

		void draw_custom_sound_picker( config::str& file_setting, std::string_view combo_label, std::string_view preview_id, float preview_volume )
		{
			const auto files = features::misc::impacts::list_custom_sounds( );

			static std::vector<std::string> cached_files{};
			static std::vector<const char*> cached_ptrs{};
			cached_files = files;
			cached_ptrs.clear( );
			cached_ptrs.reserve( cached_files.size( ) );

			for ( const auto& file : cached_files )
			{
				cached_ptrs.push_back( file.c_str( ) );
			}

			if ( !cached_ptrs.empty( ) )
			{
				auto selected{ 0 };
				for ( auto i = 0; i < static_cast< int >( cached_files.size( ) ); ++i )
				{
					if ( cached_files[ static_cast< std::size_t >( i ) ] == file_setting.value )
					{
						selected = i;
						break;
					}
				}

				if ( xui::combo( combo_label, selected, cached_ptrs.data( ), static_cast< int >( cached_ptrs.size( ) ) ) )
				{
					file_setting = cached_files[ static_cast< std::size_t >( selected ) ];
				}
			}

			xui::text_input( "File", file_setting.value, 64, "hit.wav" );

			if ( xui::button( preview_id, 96.0f, 22.0f ) )
			{
				features::misc::g_impacts.play_custom_sound( file_setting.value, preview_volume );
			}
		}
		constexpr const char* marker_types[ ]{ "Classic", "Damage", "Both" };
		constexpr const char* impact_types[ ]{ "Overlay", "Sparks", "Both" };
		constexpr const char* death_effect_types[ ]{ "Fade", "Stars" };
		constexpr auto k_death_effect_type_count{ static_cast<int>( std::size( death_effect_types ) ) };

		constexpr const char* primary_weapons[ ]{ "None", "Rifle", "Scoped rifle", "Scout", "Awp", "Auto sniper" };
		constexpr const char* secondary_weapons[ ]{ "None", "Dual elites", "Five-seven/tec-9", "Deagle", "Revolver" };
		constexpr const char* grenade_names[ ]{ "Molotov", "He grenade", "Smoke", "Flashbang", "Decoy" };

	} // namespace detail

	void menu::draw_misc( float group_w ) const
	{
		auto& m = settings::g_misc;

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
			auto& impacts = m.m_impacts;
			auto& traj = m.m_projectile_trajectory;
			auto& pen = settings::g_combat.m_penetration_crosshair;
			auto& mov = settings::g_movement;
			auto& ab = m.m_autobuy;

			if ( xui::begin_child( "impacts", col_w ) )
			{
				xui::checkbox( "Hit sound", impacts.hit_sound );
				if ( xui::begin_popup( "##hitsound_popup", 220.0f ) )
				{
					xui::combo( "Type##hs", impacts.hit_sound_type.value, detail::sound_types, detail::k_sound_type_count );

					xui::slider_float( "Volume##hs", impacts.hit_sound_volume, 1.0f, 100.0f, "%.0f%%" );

					if ( impacts.hit_sound_type.value == settings::misc::impacts::sound_type::custom )
					{
						detail::draw_custom_sound_picker( impacts.custom_hit_sound, "Sound##hs", "Preview##hs", impacts.hit_sound_volume.value );
					}

					xui::end_popup( );
				}

				xui::checkbox( "Hit marker", impacts.hit_marker );
				if ( xui::begin_popup( "##hitmarker_popup", 220.0f ) )
				{
					xui::combo( "Type##hm", impacts.hit_marker_type.value, detail::marker_types, 3 );

					xui::slider_float( "Duration##hm", impacts.hit_marker_duration, 0.1f, 5.0f, "%.1fs" );
					xui::color_picker( "Color##hm", impacts.hit_marker_color );
					xui::end_popup( );
				}

				xui::checkbox( "Hit effect", impacts.hit_effect );
				if ( xui::begin_popup( "##hitfx_popup", 220.0f ) )
				{
					xui::color_picker( "Color##hitfx", impacts.hit_effect_color );
					xui::slider_float( "Duration##hitfx", impacts.hit_effect_duration, 0.1f, 5.0f, "%.1fs" );
					xui::slider_float( "Strength##hitfx", impacts.hit_effect_strength, 1.0f, 100.0f, "%.0f%%" );
					xui::end_popup( );
				}

				xui::checkbox( "Death sound", impacts.death_sound );
				if ( xui::begin_popup( "##deathsound_popup", 220.0f ) )
				{
					xui::combo( "Type##ds", impacts.death_sound_type.value, detail::sound_types, detail::k_sound_type_count );

					xui::slider_float( "Volume##ds", impacts.death_sound_volume, 1.0f, 100.0f, "%.0f%%" );

					if ( impacts.death_sound_type.value == settings::misc::impacts::sound_type::custom )
					{
						detail::draw_custom_sound_picker( impacts.custom_death_sound, "Sound##ds", "Preview##ds", impacts.death_sound_volume.value );
					}

					xui::end_popup( );
				}

				xui::checkbox( "Death effect", impacts.death_effect );
				if ( xui::begin_popup( "##deathfx_popup", 220.0f ) )
				{
					xui::combo( "Type##deathfx", impacts.death_effect_type_sel.value, detail::death_effect_types, detail::k_death_effect_type_count );
					xui::color_picker( "Color##deathfx", impacts.death_effect_color );
					xui::end_popup( );
				}

				xui::checkbox( "Reveal score board weapons", m.m_scoreboard_weapons.enabled );
				if ( xui::begin_popup( "##scoreboardeq_popup", 220.0f ) )
				{
					xui::color_picker( "Color##scoreboardeq", m.m_scoreboard_weapons.color );
					xui::end_popup( );
				}

				xui::checkbox( "Bullet impacts", impacts.bullet_impact_effect );
				if ( xui::begin_popup( "##bulletfx_popup", 220.0f ) )
				{
					xui::combo( "Type##bulletfx", impacts.bullet_impact_effect_type.value, detail::impact_types, 3 );

					const auto type = impacts.bullet_impact_effect_type.value;
					const auto show_overlay = type == settings::misc::impacts::bullet_impact_type::overlay || type == settings::misc::impacts::bullet_impact_type::both;
					const auto show_sparks = type == settings::misc::impacts::bullet_impact_type::sparks || type == settings::misc::impacts::bullet_impact_type::both;

					if ( show_overlay )
					{
						xui::slider_float( "Duration##bulletfx", impacts.bullet_impact_effect_duration, 0.1f, 5.0f, "%.1fs" );
						xui::color_picker( "Fill##bulletfx", impacts.bullet_impact_effect_fill_color );
						xui::color_picker( "Edge##bulletfx", impacts.bullet_impact_effect_edge_color );

						xui::checkbox( "Glow##bulletfx", impacts.bullet_impact_effect_glow );
						if ( impacts.bullet_impact_effect_glow )
						{
							xui::slider_float( "Glow strength##bulletfx", impacts.bullet_impact_effect_glow_strength, 0.1f, 1.0f, "%.2f" );
						}
					}

					if ( show_sparks )
					{
						xui::color_picker( "Spark##bulletfx", impacts.bullet_impact_effect_color_spark );
					}

					xui::end_popup( );
				}

				xui::checkbox( "Bullet tracers", impacts.bullet_tracers );
				if ( xui::begin_popup( "##tracers_popup", 220.0f ) )
				{
					xui::slider_float( "Duration##tracer", impacts.bullet_tracer_duration, 0.1f, 5.0f, "%.1fs" );
					xui::color_picker( "Color##tracer", impacts.bullet_tracer_color );
					xui::slider_float( "Thickness##tracer", impacts.bullet_tracer_thickness, 0.5f, 5.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::checkbox( "Hit logs", impacts.hit_log );
				if ( xui::begin_popup( "##hitlog_popup", 220.0f ) )
				{
					xui::slider_float( "Duration##hitlog", impacts.hit_log_duration, 0.5f, 10.0f, "%.1fs" );
					xui::end_popup( );
				}

				xui::checkbox( "Miss logs", impacts.miss_log );
				if ( xui::begin_popup( "##misslog_popup", 220.0f ) )
				{
					xui::slider_float( "Duration##misslog", impacts.miss_log_duration, 0.5f, 10.0f, "%.1fs" );
					xui::end_popup( );
				}

				xui::checkbox( "Console logs", impacts.console_log );
				xui::checkbox( "Clean console", impacts.clean_console );

				// xui::checkbox( "Chat logs", m.m_chat_logs.enabled );
				// if ( xui::begin_popup( "##chatlogs_popup", 220.0f ) )
				// {
				// 	xui::checkbox( "Votekick##chatlogs", m.m_chat_logs.votekick );
				// 	xui::checkbox( "Hit logs##chatlogs", m.m_chat_logs.hit );
				// 	xui::checkbox( "Miss logs##chatlogs", m.m_chat_logs.miss );
				// 	xui::end_popup( );
				// }

				xui::end_child( );
			}

			if ( xui::begin_child( "misc visuals", col_w ) )
			{
				xui::checkbox( "Projectile trajectory", traj.enabled );
				if ( xui::begin_popup( "##traj_popup", 220.0f ) )
				{
					xui::checkbox( "Straight throw", traj.straight_throw );
					xui::color_picker( "Held color", traj.held_color );
					xui::color_picker( "Thrown color", traj.thrown_color );
					xui::color_picker( "Will damage held color", traj.will_deal_damage_held_color );
					xui::color_picker( "Will damage thrown color", traj.will_deal_damage_thrown_color );
					xui::end_popup( );
				}

				xui::checkbox( "Penetration crosshair", pen.enabled );
				if ( xui::begin_popup( "##pen_popup", 220.0f ) )
				{
					xui::checkbox( "Glow##pen", pen.glow );
					xui::slider_float( "Glow strength##pen", pen.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::color_picker( "Can penetrate##pen", pen.can_penetrate_fill );
					xui::color_picker( "Can pen outline##pen", pen.can_penetrate_outline );
					xui::color_picker( "Blocked##pen", pen.blocked_fill );
					xui::color_picker( "Blocked outline##pen", pen.blocked_outline );
					xui::end_popup( );
				}

				xui::checkbox( "Auto buy", ab.enabled );
				if ( xui::begin_popup( "##autobuy_popup", 220.0f ) )
				{
					xui::combo( "Primary##ab", ab.primary_weapon, detail::primary_weapons, 6 );
					xui::combo( "Secondary##ab", ab.secondary_weapon, detail::secondary_weapons, 5 );
					xui::checkbox( "Armor##ab", ab.armor );
					xui::checkbox( "Defuser##ab", ab.defuser );
					xui::checkbox( "Taser##ab", ab.taser );
					xui::multicombo( "Grenades##ab", ab.grenades, detail::grenade_names, 5 );
					xui::end_popup( );
				}

				auto& wm = m.m_watermark;
				xui::checkbox( "Watermark", wm.enabled );
				if ( xui::begin_popup( "##watermark_popup", 240.0f ) )
				{
					static const char* wm_anchors[] = { "top left", "top right", "bottom left", "bottom right", "top center", "bottom center" };
					xui::combo( "Anchor##wm", wm.anchor.value, wm_anchors, 6 );
					xui::slider_float( "Margin##wm", wm.margin, 0.0f, 60.0f, "%.0fpx" );
					xui::slider_float( "Scale##wm", wm.scale, 0.5f, 2.0f, "%.2fx" );
					xui::layout::separator( );
					xui::checkbox( "Name##wm", wm.show_name );
					xui::checkbox( "Specs##wm", wm.show_specs );
					xui::checkbox( "Fps##wm", wm.show_fps );
					xui::checkbox( "Ping##wm", wm.show_ping );
					xui::checkbox( "Time##wm", wm.show_time );
					xui::layout::separator( );
					xui::checkbox( "Ping warning##wm", wm.show_ping_warning );
					if ( wm.show_ping_warning.value )
					{
						xui::slider_int( "Warning above##wm", wm.ping_warning, 20, 300, "%dms" );
					}
					xui::end_popup( );
				}

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "movement", col_w ) )
			{
				xui::checkbox( "Bhop", mov.bhop );
				xui::checkbox( "Airstrafe", mov.airstrafe );

				if ( xui::begin_popup( "##airstrafe_popup", 220.0f ) )
				{
					xui::checkbox( "Directional", mov.airstrafe_fully_directional );
					xui::end_popup( );
				}

				xui::checkbox( "Subtick strafer", mov.m_test_strafer.enabled );
				xui::checkbox( "Jumpbug", mov.jumpbug );
				xui::checkbox( "Fastladder", mov.fastladder );
				xui::checkbox( "Edgejump", mov.edgejump );
				xui::checkbox( "Edgestop", mov.edgestop );
				xui::checkbox( "Edgebug", mov.edgebug );
				if ( xui::begin_popup( "##edgebug_popup", 240.0f ) )
				{
					static const char* edgebug_modes[] = { "0: loose", "1: edge trace (default)", "2: no jump held", "3: min speed", "4: strict vz" };
					xui::combo( "Mode##eb", mov.edgebug_mode.value, edgebug_modes, 5 );
					xui::slider_int( "Passes##eb", mov.edgebug_passes, 1, 5, "%d" );
					xui::checkbox( "Jump steps##eb", mov.edgebug_include_jump_steps );
					xui::end_popup( );
				}
				xui::checkbox( "Slowwalk", mov.slowwalk );

				if ( xui::begin_popup( "##slowwalk_popup", 220.0f ) )
				{
					xui::slider_float( "Speed", mov.slowwalk_speed, 1.0f, 100.0f, "%.2fs" );
					xui::end_popup( );
				}

				xui::layout::separator( );

				xui::checkbox( "Quickswitch", m.quickswitch );
				xui::checkbox( "Quick plant", m.quick_plant );
				xui::checkbox( "No land inaccuracy", m.no_land_inaccuracy );

				xui::end_child( );
			}

			if ( xui::begin_child( "misc other", col_w ) )
			{
				xui::checkbox( "Preserve killfeed", m.preserve_killfeed );
				xui::checkbox( "Auto accept", m.autoaccept );
				xui::checkbox( "Anti afk", m.anti_afk );

				xui::layout::separator( );

				xui::checkbox( "Clantag", m.m_name_changer.clantag );
				xui::checkbox( "Name Changer", m.m_name_changer.override_name );
				if ( xui::begin_popup( "##override_name_popup", 220.0f ) )
				{
					xui::text_input( "Name##nc", m.m_name_changer.name.value, 32, "Player name..." );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}

		if ( subtab == 1 )
		{
			auto& cam = m.m_camera;
			auto& vm = m.m_viewmodel_adjust;

			if ( xui::begin_child( "camera", col_w ) )
			{
				xui::checkbox( "Custom fov", cam.change_fov );
				if ( xui::begin_popup( "##fov_popup", 220.0f ) )
				{
					xui::slider_float( "Fov", cam.fov, 60.0f, 150.0f, "%.0f" );
					xui::checkbox( "Scoped fov override", cam.scoped_fov_override );
					xui::slider_float( "Scoped fov", cam.scoped_fov, 10.0f, 90.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::checkbox( "Thirdperson", cam.thirdperson );
				if ( xui::begin_popup( "##tp_popup", 220.0f ) )
				{
					xui::slider_float( "Distance", cam.thirdperson_distance, 35.0f, 200.0f, "%.0f" );
					xui::slider_float( "Hull size", cam.thirdperson_hull_size, 0.0f, 20.0f, "%.0f" );
					xui::checkbox( "Close on grenade", m.thirdperson_grenade );
					xui::end_popup( );
				}

				xui::checkbox( "Aspect ratio", cam.change_aspect_ratio );
				if ( xui::begin_popup( "##ar_popup", 220.0f ) )
				{
					xui::slider_float( "Ratio##ar", cam.aspect_ratio, 1.0f, 1.78f, "%.3f" );
					xui::end_popup( );
				}

				xui::end_child( );
			}

			if ( xui::begin_child( "hud", col_w ) )
			{
				xui::checkbox( "Crosshair overlay", m.m_hud.m_crosshair.enabled );
				if ( xui::begin_popup( "##xhair_popup", 220.0f ) )
				{
					xui::slider_float( "Size##xhair", m.m_hud.m_crosshair.size, 0.5f, 10.0f, "%.1f" );
					xui::slider_float( "Outline##xhair", m.m_hud.m_crosshair.outline, 0.0f, 4.0f, "%.1f" );
					xui::color_picker( "Color##xhair", m.m_hud.m_crosshair.color );
					xui::color_picker( "Outline color##xhair", m.m_hud.m_crosshair.outline_color );
					xui::end_popup( );
				}

				xui::checkbox( "Scope overlay", m.m_hud.m_scope.enabled );
				if ( xui::begin_popup( "##scope_popup", 220.0f ) )
				{
					xui::slider_float( "Line length", m.m_hud.m_scope.line_length, 10.0f, 500.0f, "%.0f" );
					xui::slider_float( "Gap##scope", m.m_hud.m_scope.gap, 0.0f, 50.0f, "%.0f" );
					xui::slider_float( "Thickness##scope", m.m_hud.m_scope.thickness, 0.5f, 5.0f, "%.2f" );
					xui::slider_float( "Anim speed", m.m_hud.m_scope.anim_speed, 1.0f, 30.0f, "%.0f" );
					xui::color_picker( "Color##scope", m.m_hud.m_scope.color );
					xui::checkbox( "Fade in##scope", m.m_hud.m_scope.fade_in );

					xui::layout::separator( );

					xui::checkbox( "Glow##scope", m.m_hud.m_scope.glow );
					xui::slider_float( "Glow strength##scope", m.m_hud.m_scope.glow_strength, 0.1f, 1.0f, "%.2f" );
					xui::end_popup( );
				}

				// xui::checkbox( "Sniper crosshair", m.sniper_crosshair );

				xui::end_child( );
			}

			xui::layout::set_cursor( right_x - wx, body_y - wy );

			if ( xui::begin_child( "misc viewmodel", col_w ) )
			{
				xui::checkbox( "Viewmodel fov", vm.enabled );
				if ( xui::begin_popup( "##vm_fov_popup", 220.0f ) )
				{
					xui::slider_float( "Fov", vm.fov, 54.0f, 90.0f, "%.0f" );
					xui::end_popup( );
				}

				xui::checkbox( "Viewmodel position", vm.position_enabled );
				if ( xui::begin_popup( "##vm_pos_popup", 220.0f ) )
				{
					xui::slider_float( "Offset x", vm.offset_x, -10.0f, 10.0f, "%.1f" );
					xui::slider_float( "Offset y", vm.offset_y, -10.0f, 10.0f, "%.1f" );
					xui::slider_float( "Offset z", vm.offset_z, -10.0f, 10.0f, "%.1f" );
					xui::end_popup( );
				}

				xui::checkbox( "No hand movement", vm.no_hand_movement );

				xui::end_child( );
			}

			if ( xui::begin_child( "hud extras", col_w ) )
			{
				auto& rem = m.m_removals;

				xui::checkbox( "Removals", rem.enabled );
				if ( xui::begin_popup( "##removals_popup", 220.0f ) )
				{
					xui::checkbox( "Remove crosshair", rem.crosshair );
					xui::checkbox( "Remove scope", rem.scope );
					xui::checkbox( "Remove overhead", rem.overhead );
					xui::checkbox( "Remove legs", rem.legs );
					xui::checkbox( "Remove recoil", rem.recoil );
					xui::checkbox( "Remove skybox fog", rem.skybox_fog );
					xui::checkbox( "Remove 3d skybox", rem.skybox_3d );
					xui::checkbox( "Remove decals", rem.decals );
					xui::checkbox( "Remove smoke", rem.smoke );
					xui::checkbox( "Remove flash", rem.flash );
					xui::end_popup( );
				}

				xui::end_child( );
			}
		}
	}

} // namespace rendering
