



#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>

#include "../movement.hpp"

namespace features::movement {

	namespace {

		[[nodiscard]] bool is_valid_ground(
			std::uintptr_t local_pawn,
			std::uintptr_t movement_services,
			const math::vector3& start,
			const math::vector3& end,
			const math::vector3& mins,
			const math::vector3& maxs,
			float& out_fraction)
		{
			auto trace_mask{ 0ull };
			{
				const auto pawn_ptr = memory::read<std::uintptr_t>(movement_services + 56);
				if (!pawn_ptr) return false;

				trace_mask = memory::read<std::uintptr_t>(pawn_ptr + 0xd48);
				if (memory::read<std::uint32_t>(pawn_ptr + 0x3f8) & 0x10)
					trace_mask |= 0x20;
			}

			const auto filter = systems::g_tracing.make_player_movement_filter(local_pawn, trace_mask, 11);
			const auto sv_standable_normal = CONVAR("sv_standable_normal")->get<float>();

			const auto result = systems::g_tracing.trace_player_bbox(
				start, end, { mins, maxs }, filter, movement_services
			);

			if (result.fraction <= 0.0f || result.fraction >= 1.0f)
				return false;

			if (result.normal.z < sv_standable_normal)
				return false;

			out_fraction = result.fraction;
			return true;
		}

		[[nodiscard]] std::pair<math::vector3, math::vector3> get_collision_bounds(std::uintptr_t local_pawn)
		{
			const auto collision = local_pawn + SCHEMA("C_BaseModelEntity", "m_Collision"_hash);
			return {
				memory::read<math::vector3>(collision + SCHEMA("CCollisionProperty", "m_vecMins"_hash)),
				memory::read<math::vector3>(collision + SCHEMA("CCollisionProperty", "m_vecMaxs"_hash))
			};
		}

	}

    void jumpbug::on_create_move(systems::input::usercmd* cmd)
    {
        this->m_active_this_tick = false;

        if (!settings::g_movement.jumpbug.value)
            return;

        if (features::movement::g_edgebug.active_this_tick())
            return;

        const auto local = systems::g_local.get();
        if (!local.pawn)
            return;

        const auto move_type = memory::read<std::uint8_t>(local.pawn + SCHEMA("C_BaseEntity", "m_nActualMoveType"_hash));
        if (move_type == cstypes::move_type::ladder || move_type == cstypes::move_type::noclip)
            return;

        if (!(cmd->buttons.value & cstypes::command_buttons::in_jump))
        {
            const auto& prestate = systems::g_prediction.pre();
            const bool on_ground = (prestate.flags & cstypes::entity_flags::on_ground) != 0;
            this->m_was_on_ground_last_tick = on_ground;

            if (!on_ground)
                this->m_jb_fired_this_landing = false;
            return;
        }

        const auto& prestate = systems::g_prediction.pre();
        const auto movement_services = memory::read<std::uintptr_t>(
            local.pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash)
        );
        if (!movement_services)
            return;

        const bool currently_on_ground = (prestate.flags & cstypes::entity_flags::on_ground) != 0;
        const float vel_z = prestate.networked_velocity.z;
        const float speed_2d = prestate.networked_velocity.length_2d();

        const bool was_on_ground = this->m_was_on_ground_last_tick;
        this->m_was_on_ground_last_tick = currently_on_ground;


        if (this->m_jb_fired_this_landing)
        {
            if (!currently_on_ground)
                this->m_jb_fired_this_landing = false;
            else
                return;
        }


        if (vel_z > 1.0f && !(!was_on_ground && currently_on_ground))
            return;




        if (!was_on_ground && currently_on_ground)
        {
            this->m_active_this_tick = true;
            this->m_landing_fraction = 0.0f;
        }

        else if (!currently_on_ground && vel_z < -1.0f)
        {
            auto velocity = prestate.networked_velocity;
            const auto origin = prestate.networked_origin;
            const auto [stand_mins, stand_maxs] = get_collision_bounds(local.pawn);

            auto crouch_maxs = stand_maxs;
            crouch_maxs.z = stand_mins.z + (stand_maxs.z - stand_mins.z) * 0.5f;

            const auto tick_dt = cstypes::tick_interval;
            auto trace_end = origin + velocity * tick_dt;
            trace_end.z -= 2.0f;

            float fraction{ 0.0f };
            bool valid_landing = false;

            if (is_valid_ground(local.pawn, movement_services, origin, trace_end, stand_mins, stand_maxs, fraction))
                valid_landing = true;

            if (!valid_landing &&
                is_valid_ground(local.pawn, movement_services, origin, trace_end, stand_mins, crouch_maxs, fraction))
                valid_landing = true;

            if (!valid_landing || fraction <= 0.0f || fraction > 0.95f)
                return;

            this->m_active_this_tick = true;
            this->m_landing_fraction = fraction;
        }

        else
        {
            return;
        }

        if (!this->m_active_this_tick)
            return;


        this->m_jb_fired_this_landing = true;

        logging::console::print(xs("[jumpbug] speed={:.0f} frac={:.3f} vz={:.1f} ground={}"),
            speed_2d, this->m_landing_fraction, vel_z, currently_on_ground ? "yes" : "no");

        cmd->buttons.value &= ~cstypes::command_buttons::in_jump;
        cmd->buttons.value_changed |= cstypes::command_buttons::in_jump;

        const auto base = cmd->csgo_user_cmd.mutable_base();
        if (!base)
            return;

        const auto subtick_moves = base->mutable_subtick_moves();

        if (auto* duck_down = systems::g_input.acquire_subtick_step(subtick_moves))
        {
            duck_down->set_button(cstypes::command_buttons::in_duck);
            duck_down->set_pressed(true);
            duck_down->set_when(0.0f);
            duck_down->set_analog_forward_delta(0.0f);
            duck_down->set_analog_left_delta(0.0f);
        }

        if (auto* jump_release = systems::g_input.acquire_subtick_step(subtick_moves))
        {
            jump_release->set_button(cstypes::command_buttons::in_jump);
            jump_release->set_pressed(false);
            jump_release->set_when(this->m_landing_fraction > 0.0f ? this->m_landing_fraction - 0.001f : 0.0f);
            jump_release->set_analog_forward_delta(0.0f);
            jump_release->set_analog_left_delta(0.0f);
        }

        if (auto* jump_press = systems::g_input.acquire_subtick_step(subtick_moves))
        {
            jump_press->set_button(cstypes::command_buttons::in_jump);
            jump_press->set_pressed(true);
            jump_press->set_when(this->m_landing_fraction);
            jump_press->set_analog_forward_delta(0.0f);
            jump_press->set_analog_left_delta(0.0f);
        }

        if (auto* duck_up = systems::g_input.acquire_subtick_step(subtick_moves))
        {
            duck_up->set_button(cstypes::command_buttons::in_duck);
            duck_up->set_pressed(false);
            duck_up->set_when(std::min(this->m_landing_fraction + 0.001f, 1.0f));
            duck_up->set_analog_forward_delta(0.0f);
            duck_up->set_analog_left_delta(0.0f);
        }
    }

	float jumpbug::get_impulse_mul(std::uintptr_t local_pawn) const
	{
		const auto movement_services = memory::read<std::uintptr_t>(
			local_pawn + SCHEMA("C_BasePlayerPawn", "m_pMovementServices"_hash)
		);
		if (!movement_services)
			return 0.0f;

		const auto stamina = memory::read<float>(
			movement_services + SCHEMA("CCSPlayer_MovementServices", "m_flStamina"_hash)
		);

		if (CONVAR("sv_legacy_jump")->get<bool>())
		{
			if (stamina <= 0.0f)
				return 1.0f;
			return std::clamp(1.0f - (stamina / 100.0f), 0.0f, 1.0f);
		}

		const auto current_tick = memory::read<int>(
			memory::read<std::uintptr_t>(addresses::globals::global_vars) + 0x44
		);
		const auto modern_jump = movement_services + SCHEMA("CCSPlayer_MovementServices", "m_ModernJump"_hash);
		const auto landing_vel_z = memory::read<float>(
			modern_jump + SCHEMA("CCSPlayerModernJump", "m_flLastLandedVelocityZ"_hash)
		);
		const auto landed_tick = memory::read<std::uint32_t>(
			modern_jump + SCHEMA("CCSPlayerModernJump", "m_nLastLandedTick"_hash)
		);
		const auto base = std::clamp((landing_vel_z * 0.0005f) + 1.0f, 0.02f, 1.0f);
		const auto ticks_since_landing = static_cast<float>(current_tick - landed_tick);

		auto result = std::clamp(base + (ticks_since_landing * 0.6f), 0.0f, 1.0f);

		if (stamina > 0.0f)
			result *= std::clamp(1.0f - (stamina / 100.0f), 0.0f, 1.0f);

		return result;
	}

    void jumpbug::on_level_change()
    {
        m_active_this_tick = false;
        m_landing_fraction = 0.0f;
        m_was_on_ground_last_tick = false;
        m_jb_fired_this_landing = false;
    }

}