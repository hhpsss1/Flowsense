#include <pch/pch.hpp>
#include <core/settings.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		constexpr const char* hitbox_names[ ]{ "Head", "Chest", "Stomach", "Arms", "Legs", "Paws" };
		constexpr const char* pitch_items[ ]{ "None", "Down", "Up" };
		constexpr const char* yaw_items[ ]{ "None", "Spin", "Jitter", "Jitter spin" };

	} // namespace detail

	void menu::draw_ragebot( float group_w ) const
	{
		auto& s = settings::g_combat;
		auto& rb = s.m_ragebot;
		auto& aa = s.m_antiaim;
		auto& qp = s.m_quickpeek;
		auto& dp = s.m_duckpeek;
		auto& zb = s.m_zeusbot;
		auto& kb = s.m_knifebot;
		auto& autos = s.m_autos;
		auto& lg = s.m_lagcomp;

		auto& wg = rb.groups[ this->m_subtab ];

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		const auto content_x = this->m_content_x;
		const auto body_y = this->m_body_y;
		const auto content_w = this->m_content_w;
		const auto col_w = this->m_col_w;
		const auto right_x = content_x + col_w + tokens::gap;

		xui::layout::set_cursor( content_x - wx, body_y - wy );

		if ( xui::begin_child( "ragebot aimbot", col_w ) )
		{
			xui::checkbox( "Enabled", rb.enabled );
			xui::checkbox( "Autostop", rb.autostop );
			xui::checkbox( "Autostop between shots", rb.autostop_between_shots );
			xui::checkbox( "Silent", wg.silent );
			xui::checkbox( "No spread", wg.no_spread );
			xui::checkbox( "Double tap", wg.doubletap );
			xui::checkbox( "Rapid fire", wg.rapid_fire );
			xui::checkbox( "Autoscope", autos.scope );

			xui::layout::separator( );

			xui::checkbox( "Shot timing", wg.shot_timing );
			if ( xui::begin_popup( "##shot_timing_popup", 220.0f ) )
			{
				xui::slider_int( "Lookahead ticks##st", wg.shot_timing_lookahead, 1, 16, "%d" );
				xui::slider_int( "Max hold ticks##st", wg.shot_timing_max_hold, 2, 32, "%d" );
				xui::end_popup( );
			}
			xui::checkbox( "Resolver", wg.resolver_enabled );
			xui::checkbox( "Force shot in air", wg.force_shot_air );
			xui::checkbox( "Force shot on ground", wg.force_shot );
			xui::checkbox( "Extrapolation", lg.extrapolation );
			xui::checkbox( "Autostop inair", wg.autostop_inair );

			xui::layout::separator( );

			xui::slider_float( "Max fov", wg.max_fov, 1.0f, 180.0f, "%.0f°" );
			xui::slider_int( "Hit chance", wg.hitchance, 25, 100, "%d%%" );
			xui::slider_int( "Min damage", wg.min_damage, 5, 125, "%d" );

			xui::checkbox( "Hit chance override", wg.hitchance_override );
			if ( xui::begin_popup( "##hitchance_popup", 220.0f ) )
			{
				xui::slider_int( "Value##hc", wg.hitchance_override_value, 0, 100, "%d%%" );
				xui::end_popup( );
			}

			xui::checkbox( "Min damage override", wg.min_damage_override );
			if ( xui::begin_popup( "##mindamage_popup", 220.0f ) )
			{
				xui::slider_int( "Value##md", wg.min_damage_override_value, 0, 130, "%d" );
				xui::checkbox( "Min damage hp+1", wg.min_damage_hp_plus_one );
				xui::end_popup( );
			}

			xui::end_child( );
		}

		if ( xui::begin_child( "ragebot extras", col_w ) )
		{
			xui::checkbox( "Force b-aim", wg.body_aim );
			xui::checkbox( "Dynamic point scale", wg.dynamic_pointscale );
			xui::slider_float( "Point scale", wg.pointscale, 0.0f, 100.0f, "%.0f%%" );
			xui::multicombo( "Hitboxes", wg.hitboxes, detail::hitbox_names, 6 );

			xui::checkbox( "Adaptive hit chance", wg.adaptive_hitchance );
			if ( xui::begin_popup( "##adaptive_hc_popup", 220.0f ) )
			{
				xui::slider_int( "Misses##ahc", wg.adaptive_hitchance_misses, 1, 8, "%d" );
				xui::slider_float( "Boost per step##ahc", wg.adaptive_hitchance_boost, 0.01f, 0.25f, "%.2f" );
				xui::end_popup( );
			}

			xui::checkbox( "Baim if head hc low", wg.baim_if_head_low_hc );
			if ( xui::begin_popup( "##baim_hc_popup", 220.0f ) )
			{
				xui::slider_float( "Threshold##baimhc", wg.baim_head_hc_threshold, 0.05f, 1.0f, "%.2f" );
				xui::end_popup( );
			}

			xui::checkbox( "Adaptive min damage", wg.adaptive_min_damage );
			xui::checkbox( "Target priority", wg.target_priority );
			xui::checkbox( "Safe line check", wg.safe_line_check );

			xui::end_child( );
		}

		xui::layout::set_cursor( right_x - wx, body_y - wy );

		if ( xui::begin_child( "anti aim", col_w ) )
		{
			xui::checkbox( "Anti aim", aa.enabled );

			xui::combo( "Pitch", aa.pitch.value, detail::pitch_items, 3 );
			xui::combo( "Yaw mode", aa.yaw.value, detail::yaw_items, 4 );

			if ( aa.yaw.value == settings::combat::antiaim::yaw_mode::spin || aa.yaw.value == settings::combat::antiaim::yaw_mode::jitter_spin )
			{
				xui::slider_float( "Spin speed", aa.spin_speed, 1.0f, 360.0f, "%.0f" );
			}
			if ( aa.yaw.value == settings::combat::antiaim::yaw_mode::jitter || aa.yaw.value == settings::combat::antiaim::yaw_mode::jitter_spin )
			{
				xui::slider_float( "Jitter range", aa.jitter_range, 10.0f, 180.0f, "%.0f°" );
				xui::slider_float( "Jitter speed", aa.jitter_speed, 1.0f, 30.0f, "%.0f" );
			}

			xui::checkbox( "Hide head", aa.auto_yaw_adjust );

			xui::checkbox( "Left", aa.manual_left );
			xui::checkbox( "Right", aa.manual_right );
			xui::checkbox( "Hide onshot", aa.hide_shots );
			xui::checkbox( "Avoid backstab", aa.avoid_backstab );
			xui::checkbox( "Indicator", aa.direction_indicator );

			if ( xui::begin_popup( "##aa_indicator", 220.0f ) )
			{
				xui::color_picker( "Color##aa_ind", aa.direction_indicator_color );
				xui::checkbox( "Glow##aa_ind", aa.direction_indicator_glow );
				xui::slider_float( "Glow strength##aa_ind", aa.direction_indicator_glow_strength, 0.1f, 1.0f, "%.2f" );
				xui::end_popup( );
			}

			xui::end_child( );
		}

		if ( xui::begin_child( "other bots", col_w ) )
		{
			xui::checkbox( "Auto revolver", autos.revolver );

			xui::checkbox( "Zeusbot", zb.enabled );
			if ( xui::begin_popup( "##zb_settings", 220.0f ) )
			{
				xui::slider_float( "Max fov##zb", zb.max_fov, 1, 180, "%.0f°" );
				xui::checkbox( "Drop after##zb", zb.drop_after );
				xui::end_popup( );
			}

			xui::checkbox( "Knifebot", kb.enabled );
			if ( xui::begin_popup( "##kb_settings", 220.0f ) )
			{
				xui::slider_float( "Max fov##kb", kb.max_fov, 1, 180, "%.0f°" );
				xui::end_popup( );
			}

			xui::end_child( );
		}

		if ( xui::begin_child( "peek assistance", col_w ) )
		{
			xui::checkbox( "Quick peek", qp.enabled );
			if ( xui::begin_popup( "##qp_colors", 220.0f ) )
			{
				xui::color_picker( "Base color##qp", qp.color );
				xui::color_picker( "Retracting color##qp", qp.retrack_color );
				xui::end_popup( );
			}

			xui::checkbox( "Duck peek", dp.enabled );

			xui::end_child( );
		}
	}

} // namespace rendering
