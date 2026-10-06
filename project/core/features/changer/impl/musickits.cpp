#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <core/settings.hpp>

namespace features::changer {

	void musickits::on_frame_stage_notify () {
		const auto local = systems::g_local.get ();
		if (!local.controller) {
			return;
		}

		constexpr auto k_music_id_offset {0x948};

		auto& kits = settings::g_changer.kits;

		const auto desired = (kits.music_enabled && kits.music_id > 0)
			? kits.music_id
			: 0;

		if (this->m_tracked_controller != local.controller) {
			this->m_tracked_controller = local.controller;
			this->m_last_written = -1;
		}

		const auto current = memory::read<int> (local.controller + k_music_id_offset);
		if (current == desired) {
			if (desired != 0) {
				this->m_last_written = desired;
			}
			return;
		}

		if (desired != 0) {
			memory::write<int> (local.controller + k_music_id_offset, desired);
			this->m_last_written = desired;

			static auto last_logged {-1};
			if (last_logged != desired) {
				last_logged = desired;
				logging::console::print (xs ("[music] equipped kit {}"), desired);
			}
			return;
		}

		if (this->m_last_written != -1) {
			memory::write<int> (local.controller + k_music_id_offset, 0);
			this->m_last_written = -1;
		}
	}

}
