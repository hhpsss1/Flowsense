#include <pch/pch.hpp>
#include <core/settings.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		constexpr const char* hitbox_names[ ]{ "Head", "Chest", "Stomach", "Arms", "Legs", "Paws" };
		constexpr const char* pitch_items[ ]{ "None", "Down", "Up", "Zero" };
		constexpr const char* yaw_items[ ]{ "Fixed" };
		constexpr const char* prefer_items[ ]{ "Damage", "Health", "Distance", "FOV" };
		constexpr const char* refine_items[ ]{ "All records", "Newest only" };

	}

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

		auto& wg = rb.groups[ std::clamp( this->m_subtab, 0, static_cast< int >( settings::combat::ragebot::k_group_count ) - 1 ) ];

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		const auto content_x = this->m_content_x;
		const auto body_y = this->m_body_y;
		const auto content_w = this->m_content_w;
		const auto col_w = this->m_col_w;
		const auto right_x = content_x + col_w + tokens::gap;

		xui::layout::set_cursor( content_x - wx, body_y - wy );

		if ( xui::begin_child( "Aimbot##ragebot_aimbot", col_w ) )
		{
			xui::checkbox( "Enabled", rb.enabled );
			xui::checkbox( "Silent", wg.silent );
			xui::checkbox( "No spread", wg.no_spread );

			xui::slider_float( "Max fov", wg.max_fov, 1.0f, 180.0f, "%.0f°" );

			xui::slider_int( "Hit chance", wg.hitchance, 25, 100, "%d%%" );
			xui::checkbox( "Hit chance override", wg.hitchance_override );
			if ( xui::begin_popup( "##hitchance_popup", 220.0f ) )
			{
				xui::slider_int( "Value##hc", wg.hitchance_override_value, 0, 100, "%d%%" );
				xui::end_popup( );
			}

			xui::slider_int( "Min damage", wg.min_damage, 5, 125, "%d" );
			xui::checkbox( "Min damage override", wg.min_damage_override );
			if ( xui::begin_popup( "##mindamage_popup", 220.0f ) )
			{
				xui::slider_int( "Value##md", wg.min_damage_override_value, 0, 130, "%d" );
				xui::end_popup( );
			}

			xui::end_child( );
		}

		if ( xui::begin_child( "Shots##ragebot_shots", col_w ) )
		{
			xui::checkbox( "Force shot on ground", wg.force_shot );
			xui::checkbox( "Force shot in air", wg.force_shot_air );
			xui::checkbox( "Force b-aim", wg.body_aim );
			xui::checkbox( "Double tap", wg.double_tap );
			xui::slider_int( "Burst delay", wg.burst_delay, 0, 500, "%dms" );
			xui::checkbox( "Shot timing", wg.shot_timing );
			if ( xui::begin_popup( "##shot_timing", 220.0f ) )
			{
				xui::slider_int( "Lookahead##st", wg.shot_timing_lookahead, 1, 32, "%d" );
				xui::slider_int( "Max hold##st", wg.shot_timing_max_hold, 1, 32, "%d" );
				xui::end_popup( );
			}
			xui::checkbox( "Safe line check", wg.safe_line_check );

			xui::checkbox( "Autostop", wg.autostop );
			xui::checkbox( "Autostop inair", wg.autostop_inair );
			xui::checkbox( "Autostop between shots", rb.autostop_between_shots );

			xui::end_child( );
		}

		if ( xui::begin_child( "Accuracy##ragebot_extras", col_w ) )
		{
			xui::checkbox( "Dynamic point scale", wg.dynamic_pointscale );
			xui::slider_float( "Point scale", wg.pointscale, 0.0f, 100.0f, "%.0f%%" );
			xui::multicombo( "Hitboxes", wg.hitboxes, detail::hitbox_names, 6 );
			xui::checkbox( "Autowall", wg.autowall );
			xui::multicombo( "Multipoint", wg.multipoint, detail::hitbox_names, 6 );
			xui::combo( "Prefer", wg.prefer.value, detail::prefer_items, 4 );
			xui::checkbox( "Prefer head", wg.prefer_head );
			xui::checkbox( "Baim if head hc low", wg.baim_if_head_low_hc );
			if ( xui::begin_popup( "##baim_hc", 220.0f ) )
			{
				xui::slider_float( "Threshold##baim", wg.baim_head_hc_threshold, 0.05f, 0.9f, "%.2f" );
				xui::end_popup( );
			}
			xui::combo( "Refine shot", wg.refine_shot.value, detail::refine_items, 2 );
			xui::checkbox( "Adaptive hitchance", wg.adaptive_hitchance );
			if ( xui::begin_popup( "##adaptive_hc", 220.0f ) )
			{
				xui::slider_int( "Misses##ahc", wg.adaptive_hitchance_misses, 1, 10, "%d" );
				xui::slider_float( "Boost##ahc", wg.adaptive_hitchance_boost, 0.01f, 0.25f, "%.2f" );
				xui::end_popup( );
			}
			xui::checkbox( "Debug multipoints", wg.debug_multipoints );

			xui::end_child( );
		}

		xui::layout::set_cursor( right_x - wx, body_y - wy );

		if ( xui::begin_child( "Antiaim##ragebot_antiaim", col_w ) )
		{
			xui::checkbox( "Anti aim", aa.enabled );

			xui::combo( "Pitch", aa.pitch.value, detail::pitch_items, 4 );
			xui::combo( "Yaw mode", aa.yaw_mode.value, detail::yaw_items, 1 );

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

		if ( xui::begin_child( "Lagcomp##ragebot_lagcomp", col_w ) )
		{
			xui::checkbox( "Backtrack", lg.backtrack );
			xui::slider_int( "Max backtrack ticks", lg.max_backtrack_ticks, 1, 16, "%d" );
			xui::checkbox( "Extrapolation", lg.extrapolation );
			xui::slider_int( "Max extrapolate ticks", lg.max_extrapolate_ticks, 0, 16, "%d" );

			xui::end_child( );
		}

		if ( xui::begin_child( "Other##ragebot_otherbots", col_w ) )
		{
			xui::checkbox( "Master autostop", rb.autostop );
			xui::checkbox( "Auto scope", autos.scope );

			xui::checkbox( "Auto revolver", autos.revolver );
			xui::checkbox( "Revolver quick", autos.revolver_quick );

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

		if ( xui::begin_child( "Peek##ragebot_peek", col_w ) )
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

}
