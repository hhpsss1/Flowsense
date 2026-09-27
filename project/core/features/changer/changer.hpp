#pragma once

#include <filesystem>
#include <core/systems/systems.hpp>
#include <utilities/memory/memory.hpp>
#include <protection/game_addresses.hpp>
#include <core/settings.hpp>

namespace features::changer {

	namespace detail {

		inline static std::uint32_t float_bits (float value)
		{
			std::uint32_t bits {};
			std::memcpy (&bits, &value, sizeof (bits));
			return bits;
		}

		struct cosmetic_config
		{
			int paint_kit_id {};
			std::uint32_t wear_bits {};
			int seed {};
			bool stattrak {};
			int stattrak_value {};
			std::string nametag {};
			bool paint_color {};
			std::array<std::uint32_t, 4> paint_colors {};
			std::array<settings::changer::sticker_slot, 5> stickers {};
			settings::changer::keychain_slot keychain {};

			bool operator==( const cosmetic_config& other ) const
			{
				return paint_kit_id == other.paint_kit_id
					&& wear_bits == other.wear_bits
					&& seed == other.seed
					&& stattrak == other.stattrak
					&& stattrak_value == other.stattrak_value
					&& nametag == other.nametag
					&& paint_color == other.paint_color
					&& paint_colors == other.paint_colors
					&& stickers == other.stickers
					&& keychain == other.keychain;
			}
		};

		inline static cosmetic_config make_cosmetic_config (const settings::changer::applied_skin& skin)
		{
			cosmetic_config result
			{
				.paint_kit_id = skin.paint_kit_id,
				.wear_bits = float_bits( skin.wear ),
				.seed = skin.seed,
				.stattrak = skin.stattrak,
				.stattrak_value = skin.stattrak_value,
				.nametag = skin.nametag,
				.paint_color = skin.paint_color,
				.stickers = skin.stickers,
				.keychain = skin.keychain
			};

			for ( auto i = 0ull; i < result.paint_colors.size( ); ++i )
			{
				result.paint_colors[ i ] = skin.paint_colors[ i ].val;
			}

			return result;
		}

		// Tracks the last applied cosmetic state together with the entity handle
		// so fresh weapon entities (new round) always get their attributes re-written.
		struct cosmetic_state
		{
			std::uint32_t handle {};
			cosmetic_config config {};
		};

		// Implemented in impl/stickers.cpp
		void apply_sticker_keychain_attributes (std::uintptr_t item_view, const settings::changer::applied_skin& skin);
		void clear_sticker_keychain_attributes (std::uintptr_t item_view);
		void apply_stattrak_attributes (std::uintptr_t item_view, bool enabled, int value = 0);
		void add_nametag_entity (std::uintptr_t weapon, std::uintptr_t item_view);
		void add_stattrak_entity (std::uintptr_t weapon, std::uintptr_t item_view);

		// Idempotent keycharm sync for one weapon entity. Handles the game's
		// deferred spawn/removal internally: never double-queues a module while
		// the previous spawn is still materializing, and respawns only when the
		// live module is missing or the requested config changed.
		void sync_keychain (std::uint32_t weapon_handle, std::uintptr_t weapon, std::uintptr_t item_view, const settings::changer::keychain_slot& kc);

		void update_composite_material (std::uintptr_t weapon);

		inline static std::uint32_t murmurhash2_lower (const char* str, int len, std::uint32_t seed)
		{
			constexpr auto m {0x5bd1e995};
			constexpr auto r {24};

			auto h = seed ^ len;
			auto i {0};

			while (len >= 4)
			{
				auto k =
					static_cast<std::uint32_t> ((str [i] >= 'A' && str [i] <= 'Z') ? str [i] + 32 : str [i]) |
					(static_cast<std::uint32_t> ((str [i + 1] >= 'A' && str [i + 1] <= 'Z') ? str [i + 1] + 32 : str [i + 1]) << 8) |
					(static_cast<std::uint32_t> ((str [i + 2] >= 'A' && str [i + 2] <= 'Z') ? str [i + 2] + 32 : str [i + 2]) << 16) |
					(static_cast<std::uint32_t> ((str [i + 3] >= 'A' && str [i + 3] <= 'Z') ? str [i + 3] + 32 : str [i + 3]) << 24);

				k *= m;
				k ^= k >> r;
				k *= m;

				h *= m;
				h ^= k;

				i += 4;
				len -= 4;
			}

			switch (len)
			{
			case 3: h ^= static_cast<std::uint32_t> ((str [i + 2] >= 'A' && str [i + 2] <= 'Z') ? str [i + 2] + 32 : str [i + 2]) << 16; [[fallthrough]];
			case 2: h ^= static_cast<std::uint32_t> ((str [i + 1] >= 'A' && str [i + 1] <= 'Z') ? str [i + 1] + 32 : str [i + 1]) << 8; [[fallthrough]];
			case 1: h ^= static_cast<std::uint32_t> ((str [i] >= 'A' && str [i] <= 'Z') ? str [i] + 32 : str [i]); h *= m;
			}

			h ^= h >> 13;
			h *= m;
			h ^= h >> 15;

			return h;
		}

		inline static std::uint32_t make_subclass_token (std::int16_t def_index)
		{
			const auto s = std::to_string (def_index);
			return murmurhash2_lower (s.c_str (), static_cast<int>(s.length ()), 0x31415926);
		}

		inline static std::uintptr_t find_hud_element (const char* name)
		{
			return memory::call<std::uintptr_t> (PATTERN (patterns::find_hud_element), name);
		}

		inline static void clear_hud_weapon_icon (std::uintptr_t hud_weapons, std::int32_t index, std::int64_t unk)
		{
			memory::call<std::int64_t> (PATTERN (patterns::clear_hud_weapon_icon), hud_weapons, index, unk);
		}

		struct hud_weapon_panel_t
		{
			std::uintptr_t base {};
			std::uintptr_t data {};
			std::int32_t count {};
		};

		inline static bool resolve_weapon_panel (hud_weapon_panel_t& out)
		{
			const auto hud = find_hud_element ("HudWeaponSelection");
			if (!hud)
			{
				return false;
			}

			out.base = hud - 0x98;
			out.data = memory::read<std::uintptr_t> (out.base + 0x58);
			out.count = memory::read<std::int32_t> (out.base + 0x50);
			return out.data && out.count > 0 && out.count <= 64;
		}

		inline static void clear_hud_weapon_icons ()
		{
			hud_weapon_panel_t panel;
			if (!resolve_weapon_panel (panel))
			{
				return;
			}

			for (auto i = panel.count - 1; i >= 0; --i)
			{
				clear_hud_weapon_icon (panel.base, i, 0);
			}
		}

		inline static void clear_hud_weapon_icon_for (std::uintptr_t weapon)
		{
			if (!weapon)
			{
				return;
			}

			hud_weapon_panel_t panel;
			if (!resolve_weapon_panel (panel))
			{
				return;
			}

			for (auto i = panel.count - 1; i >= 0; --i)
			{
				const auto handle = memory::read<std::int32_t> (panel.data + 72 * i + 0x38);
				if (handle < 0)
				{
					continue;
				}

				if (systems::g_entities.lookup (static_cast<std::uint32_t> (handle)) == weapon)
				{
					clear_hud_weapon_icon (panel.base, i, 0);
					return;
				}
			}
		}

		inline static void regenerate_skins ()
		{
			memory::call<void> (PATTERN (patterns::regenerate_weapon_skins));
		}

		inline static std::uintptr_t regenerate_weapon_skin_fn ()
		{
			// The per-weapon skin regen (C_CSWeaponBase::RegenerateWeaponSkin) is
			// found by tracing the per-item call inside RegenerateWeaponSkins:
			//   +0x75: 33 D2 48 8B CB E8 <rel32>  -> regen(weapon, false)
			// This survives builds where the standalone signature is missing.
			static const auto result = [] () -> std::uintptr_t
			{
				const auto plural = PATTERN (patterns::regenerate_weapon_skins);
				if (!plural)
				{
					return 0;
				}

				constexpr std::uint8_t signature[] = { 0x33, 0xD2, 0x48, 0x8B, 0xCB, 0xE8 };
				constexpr std::size_t window = 0x100;

				for (auto i = 0ull; i < window - sizeof (signature); ++i)
				{
					bool matched = true;
					for (auto j = 0ull; j < sizeof (signature); ++j)
					{
						if (memory::read<std::uint8_t> (plural + i + j) != signature[j])
						{
							matched = false;
							break;
						}
					}

					if (matched)
					{
						const auto call_site = plural + i + sizeof (signature);
						const auto displacement = memory::read<std::int32_t> (call_site);
						return call_site + 4 + static_cast<std::uintptr_t> (displacement);
					}
				}

				return 0;
			} ();

			return result;
		}

		inline static std::uintptr_t get_hud_weapon (std::uintptr_t weapon, std::uintptr_t pawn)
		{
			if (!weapon || !pawn)
			{
				return 0;
			}

			const auto arms_handle = memory::read<std::uint32_t> (pawn + SCHEMA ("C_CSPlayerPawn", "m_hHudModelArms"_hash));
			if (!arms_handle)
			{
				return 0;
			}

			const auto hud_arms = systems::g_entities.lookup (arms_handle);
			if (!hud_arms)
			{
				return 0;
			}

			const auto arms_node = memory::read<std::uintptr_t> (hud_arms + SCHEMA ("C_BaseEntity", "m_pGameSceneNode"_hash));
			if (!arms_node)
			{
				return 0;
			}

			for (auto vm = memory::read<std::uintptr_t> (arms_node + SCHEMA ("CGameSceneNode", "m_pChild"_hash));
				vm && vm > 0x10000;
				vm = memory::read<std::uintptr_t> (vm + SCHEMA ("CGameSceneNode", "m_pNextSibling"_hash)))
			{
				const auto vm_owner = memory::read<std::uintptr_t> (vm + SCHEMA ("CGameSceneNode", "m_pOwner"_hash));
				if (!vm_owner || vm_owner <= 0x10000)
				{
					continue;
				}

				const auto owner_handle = memory::read<std::uint32_t> (vm_owner + SCHEMA ("C_BaseEntity", "m_hOwnerEntity"_hash));
				if (!owner_handle)
				{
					continue;
				}

				if (systems::g_entities.lookup (owner_handle) == weapon)
				{
					return vm_owner;
				}
			}

			return 0;
		}

	} // namespace detail

	class econ_item_system {
	public:
		enum class item_category : std::uint8_t {
			gun,
			knife,
			glove,
			agent,
			other
		};

		struct paint_kit {
			int id {};
			std::string name {};
			std::string desc_token {};
			std::string name_token {};
			std::string localized_name {};
			float wear_min {};
			float wear_max {};
			bool legacy_model {};
			std::uint8_t rarity {};
		};

		struct item_def {
			std::int16_t def_index {};
			std::string item_class {};
			std::string name {};
			std::string localized_name {};
			std::string model_player {};
			std::string image_inventory {};
			int loadout_slot {};
			std::uint32_t used_by_classes {};
			item_category category {};
			std::uint8_t rarity {};

			[[nodiscard]] int team () const {
				if ((this->used_by_classes & 0xc) == 0xc) {
					return 0;
				}

				if (this->used_by_classes & 4) {
					return 2;
				}

				if (this->used_by_classes & 8) {
					return 3;
				}

				return 0;
			}
		};

		struct skin_entry {
			std::int16_t def_index {};
			int paint_kit_id {};
		};

		struct kit_entry {
			int id {};
			std::string name {};
			std::string localized_name {};
			std::string image_inventory {};
			std::uint8_t rarity {};
		};

		struct skin_image {
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv {};
			int width {};
			int height {};
		};

		[[nodiscard]] bool initialize ();

		[[nodiscard]] const std::vector<paint_kit>& paint_kits () const {
			return this->m_paint_kits;
		}
		[[nodiscard]] const std::vector<item_def>& item_defs () const {
			return this->m_item_defs;
		}

		[[nodiscard]] const std::vector<const item_def*>& knives () const {
			return this->m_knives;
		}
		[[nodiscard]] const std::vector<const item_def*>& gloves () const {
			return this->m_gloves;
		}
		[[nodiscard]] const std::vector<const item_def*>& agents () const {
			return this->m_agents;
		}
		[[nodiscard]] const std::vector<const item_def*>& guns () const {
			return this->m_guns;
		}
		[[nodiscard]] const std::vector<kit_entry>& stickers () const {
			return this->m_stickers;
		}
		[[nodiscard]] const std::vector<kit_entry>& keychains () const {
			return this->m_keychains;
		}
		[[nodiscard]] const std::vector<kit_entry>& music_kits () const {
			return this->m_music_kits;
		}
		[[nodiscard]] const std::vector<skin_entry>& skins () const {
			return this->m_skins;
		}

		[[nodiscard]] const item_def* find_def (std::int16_t def_index) const;
		[[nodiscard]] const paint_kit* find_paint_kit (int id) const;
		[[nodiscard]] const skin_image* get_skin_image (const std::string& image_inventory);
		[[nodiscard]] const skin_image* get_skin_image (std::int16_t def_index, int paint_kit_id);

		[[nodiscard]] int combined_rarity (std::int16_t def_index, int paint_kit_id) const;

		void flush_skin_images ();

	private:
		enum class image_state : std::uint8_t {
			idle,
			loading,
			decoded,
			ready,
			failed
		};

		struct image_entry {
			skin_image image {};
			std::atomic<image_state> state {image_state::idle};
			std::vector<std::vector<std::uint8_t>> mip_buffers {};
			std::uint32_t width {};
			std::uint32_t height {};
			DXGI_FORMAT format {DXGI_FORMAT_UNKNOWN};
		};

		bool parse_item_defs (std::uintptr_t schema);
		bool parse_paint_kits (std::uintptr_t schema);
		void build_indices ();
		void resolve_localized_names ();
		bool build_vpk_index ();
		void build_skin_index ();
		void parse_kit_defs ();
		void parse_music_kits (std::uintptr_t schema);
		void dump_sticker_keychain_defs ();

		void request_decode (const std::string& image_inventory);
		bool finalize_texture (image_entry& entry);

		[[nodiscard]] item_category classify (const char* item_class, int loadout_slot);
		[[nodiscard]] std::string resolve_kit_name (const kit_entry& kit) const;
		[[nodiscard]] std::vector<std::byte> read_vpk (const std::string& path);
		[[nodiscard]] bool decode_vtex (std::span<const std::byte> data, image_entry& out);
		[[nodiscard]] std::string build_skin_image_path (const item_def* def, const paint_kit* pk) const;

		std::vector<paint_kit> m_paint_kits {};
		std::vector<item_def> m_item_defs {};

		std::vector<const item_def*> m_knives {};
		std::vector<const item_def*> m_gloves {};
		std::vector<const item_def*> m_agents {};
		std::vector<const item_def*> m_guns {};
		std::vector<kit_entry> m_stickers {};
		std::vector<kit_entry> m_keychains {};
		std::vector<kit_entry> m_music_kits {};
		std::vector<skin_entry> m_skins {};

		std::unordered_map<std::int16_t, std::size_t> m_def_index_map {};
		std::unordered_map<int, std::size_t> m_paint_kit_map {};

		struct vpk_file_entry {
			std::uint16_t archive_index {};
			std::uint32_t offset {};
			std::uint32_t length {};
			std::vector<std::byte> preload {};
		};

		std::unordered_map<std::string, vpk_file_entry> m_vpk_index {};
		bool m_vpk_indexed {};
		std::filesystem::path m_vpk_directory {};

		std::unordered_map<std::string, std::unique_ptr<image_entry>> m_image_cache {};
		std::mutex m_image_mutex {};

		std::unordered_map<std::uint16_t, std::ifstream> m_archive_handles {};
		std::mutex m_vpk_mutex {};
	};

	class agents {
	public:
		void on_frame_stage_notify ();

	private:
		void cycle_weapon_owners (std::uintptr_t pawn);

		std::string m_original_model {};
		std::uintptr_t m_tracked_pawn {};
		std::uintptr_t m_applied_handle {};
		std::int16_t m_applied_def {};
		bool m_overridden {};
		int m_tracked_team {};
	};

	// Keeps the local player controller's music kit id in sync with the menu
	// selection (client-side only, same approach as Vantix: writes the
	// CCSPlayerController::m_unMusicID netvar whenever it drifts).
	class musickits {
	public:
		void on_frame_stage_notify ();

	private:
		std::uintptr_t m_tracked_controller {};
		int m_last_written {-1};
	};

	class gloves {
	public:
		void on_frame_stage_notify ();

	private:
		struct original_state {
			std::uint16_t def_index {};
			std::uint64_t item_id {};
			std::uint32_t id_high {};
			std::uint32_t id_low {};
			std::uint32_t account_id {};
			bool restore_custom_material {};
			bool initialized {};
			bool disallow_soc {};
			bool captured {};
		};

		struct attribute_state {
			float value {};
			bool present {};
		};

		[[nodiscard]] bool capture_original (std::uintptr_t item_view);
		[[nodiscard]] bool read_paint_attributes (std::uintptr_t item_view, std::array<attribute_state, 3>& attributes) const;
		[[nodiscard]] bool restore_paint_attributes (std::uintptr_t item_view) const;
		[[nodiscard]] bool paint_attributes_match (std::uintptr_t item_view, const settings::changer::applied_skin& skin) const;
		void apply (std::uintptr_t pawn, std::uintptr_t item_view, int team, const econ_item_system::item_def& def, const settings::changer::applied_skin& skin, std::uint32_t account_id);
		void restore (std::uintptr_t pawn, std::uintptr_t item_view, int team);
		void refresh (std::uintptr_t pawn, std::uintptr_t item_view, int team) const;
		void reset ();

		original_state m_original {};
		std::array<attribute_state, 3> m_original_attributes {};
		std::uintptr_t m_tracked_pawn {};
		int m_tracked_team {};
		bool m_overridden {};
	};

	class guns {
	public:
		void on_frame_stage_notify ();

	private:
		void apply (std::uintptr_t weapon, std::uintptr_t iv, std::uintptr_t pawn, const settings::changer::applied_skin* skin, std::int16_t def_index, std::uint32_t handle, bool& did_update);
		void update_view_model (std::uintptr_t weapon, std::uintptr_t pawn, const econ_item_system::paint_kit* pk);

		std::uint32_t m_last_active_handle {};
		std::uintptr_t m_tracked_pawn {};
		std::unordered_map<std::int16_t, bool> m_last_paint_color {};
		std::unordered_map<std::int16_t, detail::cosmetic_state> m_last_cosmetic {};
	};

	class knives {
	public:
		void on_frame_stage_notify ();

	private:
		struct original_state {
			std::uint16_t def_index {};
			std::uint32_t id_high {};
			std::uint32_t id_low {};
			std::uint32_t account_id {};
			bool initialized {};
			int paint_kit {};
			int seed {};
			float wear {};
			int stattrak {};
			bool captured {};
		};

		void capture_original (std::uintptr_t weapon, std::uintptr_t iv);
		void apply (std::uintptr_t weapon, std::uintptr_t iv, const econ_item_system::item_def* def, const settings::changer::applied_skin* skin, std::uintptr_t pawn, std::uint32_t handle, bool& did_update);
		void restore (std::uintptr_t weapon, std::uintptr_t iv, std::uintptr_t pawn);
		void update_view_model (std::uintptr_t weapon, std::uintptr_t pawn, const econ_item_system::paint_kit* pk);

		original_state m_original {};
		std::uint32_t m_last_active_handle {};
		std::uintptr_t m_tracked_pawn {};
		bool m_overridden {};
		bool m_last_paint_color {};
		detail::cosmetic_state m_last_cosmetic {};
	};

} // namespace features::changer
