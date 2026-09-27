#include <pch/pch.hpp>
#include <core/settings.hpp>

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		constexpr const char* hitbox_names_legit[ ]{ "Head", "Chest", "Stomach", "Arms", "Legs" };
		constexpr const char* enable_after_options[ ]{ "Always", "After 1st shot", "After 2nd shot" };

	} // namespace detail

	void menu::draw_legitbot( float group_w ) const
	{
		auto& s = settings::g_combat;
		auto& lb = s.m_legitbot;
		auto& wg = lb.groups[ this->m_subtab ];

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		const auto content_x = this->m_content_x;
		const auto body_y = this->m_body_y;
		const auto content_w = this->m_content_w;
		const auto col_w = this->m_col_w;
		const auto right_x = content_x + col_w + tokens::gap;

		xui::layout::set_cursor( content_x - wx, body_y - wy );

		if ( xui::begin_child( "legitbot master", col_w ) )
		{
			xui::checkbox( "Enabled", lb.enabled );
			xui::end_child( );
		}

		if ( xui::begin_child( "legitbot aim", col_w ) )
		{
			xui::checkbox( "Aimbot", wg.aimbot );
			if ( xui::begin_popup( "##aimbot_popup", 220.0f ) )
			{
				xui::combo( "Enable after##aimbot", wg.aimbot_enable_after.value, detail::enable_after_options, 3 );
				xui::end_popup( );
			}

			xui::slider_float( "Fov", wg.fov, 0.5f, 30.0f, "%.1f°" );
			xui::slider_int( "Smooth", wg.smooth, 0, 100, "%d" );
			xui::slider_float( "Aim speed", wg.aim_speed, 1.0f, 100.0f, "%.1f" );
			xui::slider_float( "Aim speed (attack)", wg.aim_speed_attack, 1.0f, 100.0f, "%.1f" );
			xui::slider_float( "Speed scale fov", wg.speed_scale_fov, 0.0f, 3.0f, "%.2f" );
			xui::slider_float( "Reaction time", wg.reaction_time, 0.0f, 1.0f, "%.2fs" );
			xui::slider_float( "Max lock-on", wg.max_lock_on, 0.0f, 5.0f, "%.1fs" );
			xui::multicombo( "Hitboxes", wg.hitboxes, detail::hitbox_names_legit, 5 );

			xui::checkbox( "Visible check", wg.visible_check );
			xui::slider_float( "Switch target delay", wg.switch_target_delay, 0.0f, 2.0f, "%.1fs" );
			xui::checkbox( "Aim through smoke", wg.aim_through_smoke );
			xui::checkbox( "Aim while flashed", wg.aim_while_flashed );
			xui::checkbox( "Autoscope", wg.autoscope );
			// xui::checkbox( "Silent aim", wg.silent_aim );

			xui::checkbox( "Visualize fov", wg.visualize_fov );

			if ( xui::begin_popup( "##fov_color_popup", 220.0f ) )
			{
				xui::color_picker( "Color##fov", wg.fov_color );
				xui::end_popup( );
			}

			xui::end_child( );
		}

		xui::layout::set_cursor( right_x - wx, body_y - wy );

		if ( xui::begin_child( "triggerbot", col_w ) )
		{
			xui::checkbox( "Triggerbot", wg.triggerbot );
			xui::slider_int( "Delay", wg.trigger_delay, 0, 250, "%d ms" );
			xui::slider_int( "Hitchance", wg.trigger_hitchance, 0, 100, "%d%%" );
			xui::slider_int( "Min damage##trigger", wg.trigger_min_damage, 1, 125, "%d" );
			xui::checkbox( "Always on", wg.trigger_always_on );
			xui::checkbox( "Autostop", wg.trigger_autostop );

			xui::end_child( );
		}

		if ( xui::begin_child( "legitbot extras", col_w ) )
		{
			xui::checkbox( "Autowall", wg.autowall );
			if ( xui::begin_popup( "##aw_popup", 220.0f ) )
			{
				xui::slider_int( "Min damage##aw", wg.min_damage, 1, 125, "%d" );
				xui::end_popup( );
			}
			xui::checkbox( "No spread", wg.manual_nospread );

			xui::end_child( );
		}
	}

} // namespace rendering
