#include <pch/pch.hpp>
#include <cstring>
#include <utilities/memory/memory.hpp>
#include <utilities/logging/logging.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
#include "../changer.hpp"

// Sticker / keychain attributes are applied the same way gloves are:
// SetOrAddAttributeValueByName on the item view's dynamic attribute list.
// Integer values (sticker id, schema, keychain id, keychain seed) are stored
// as float32 bit-casts, exactly like the working server-side plugins do
// (ViewAsFloat(id)). An id of 0 clears the slot.

namespace features::changer::detail {

	namespace {

		constexpr std::size_t sticker_slot_count {5};

		// Runtime C_KeychainModule vftable (resolved via RTTI on first use).
		std::uintptr_t keychain_module_vtable {};

		// Reads a NUL-terminated C string through a possibly-bad pointer.
		[[nodiscard]] bool try_read_string (std::uintptr_t address, char* out, std::size_t capacity)
		{
			if (!address || address <= 0x10000)
			{
				return false;
			}

			__try
			{
				const auto* str = reinterpret_cast<const char*> (address);
				auto i {0u};
				while (i + 1 < capacity && str [i])
				{
					out [i] = str [i];
					++i;
				}
				out [i] = '\0';
				return true;
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				return false;
			}
		}

		void dump_name_hashes ();

		void ensure_keychain_vtable ()
		{
			if (keychain_module_vtable)
			{
				return;
			}

			keychain_module_vtable = memory::find_vtable_by_rtti (memory::get_module_base ("client.dll"), "C_KeychainModule");
			logging::console::print (xs ("[kcdbg] vtable resolved = {:#x}"), keychain_module_vtable);
			dump_name_hashes ();
		}

		// The keychain module spawns parented to the weapon model, so it shows
		// up as a child in the weapon's game scene node tree. Walk that chain
		// and match the owner entity's vtable - the same trick the reference
		// implementation uses to detect an already-attached charm.
		[[nodiscard]] std::uintptr_t find_keychain_module (std::uintptr_t weapon)
		{
			ensure_keychain_vtable ();
			if (!weapon || !keychain_module_vtable)
			{
				return 0;
			}

			const auto node = memory::read<std::uintptr_t> (weapon + SCHEMA ("C_BaseEntity", "m_pGameSceneNode"_hash));
			if (!node)
			{
				return 0;
			}

			auto child = memory::read<std::uintptr_t> (node + SCHEMA ("CGameSceneNode", "m_pChild"_hash));
			while (child && child > 0x10000)
			{
				const auto owner = memory::read<std::uintptr_t> (child + SCHEMA ("CGameSceneNode", "m_pOwner"_hash));
				if (owner && owner > 0x10000 && memory::read<std::uintptr_t> (owner) == keychain_module_vtable)
				{
					return owner;
				}

				child = memory::read<std::uintptr_t> (child + SCHEMA ("CGameSceneNode", "m_pNextSibling"_hash));
			}

			return 0;
		}

		[[nodiscard]] float bits_to_float (std::uint32_t bits)
		{
			float value {};
			std::memcpy (&value, &bits, sizeof (value));
			return value;
		}		void set_attribute (std::uintptr_t item_view, const char* name, float value)
		{
			const auto set = PATTERN (patterns::econ_item_view_set_attribute);
			if (!set)
			{
				return;
			}

			memory::call<void> (set, item_view, name, value);
		}

		void set_int_attribute (std::uintptr_t item_view, const char* name, int value)
		{
			set_attribute (item_view, name, bits_to_float (static_cast<std::uint32_t>(value)));
		}

		// Ground-truth check: dump the item view's dynamic attribute list so we
		// can see whether our writes actually land under the names we use.
		void dump_item_attributes (std::uintptr_t item_view)
		{
			const auto list = item_view + SCHEMA ("C_EconItemView", "m_AttributeList"_hash);
			if (!list)
			{
				logging::console::print (xs ("[attrdump] no m_AttributeList schema"));
				return;
			}

			char hex[3 * 64] {};
			std::size_t off {};
			__try
			{
				for (auto i = 0u; i < 48 && off + 3 < sizeof (hex); ++i)
				{
					const auto b = memory::read<std::uint8_t> (list + i);
					static constexpr char digits[] = "0123456789abcdef";
					hex [off++] = digits [(b >> 4) & 0xF];
					hex [off++] = digits [b & 0xF];
					hex [off++] = ' ';
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
			hex [off] = '\0';

			logging::console::print (xs ("[attrdump] iv={:#x} list={:#x} header: {}"), item_view, list, hex);

			// Header layout observed: {static_ptr, count, data*, capacity}.
			// Follow the data pointer and search every dword for the murmur
			// hashes of the attribute names we write; print hits with the
			// following float/u32 payload so layout can be confirmed.
			const auto count = memory::read<std::int32_t> (list + 8);
			const auto data = memory::read<std::uintptr_t> (list + 16);
			if (!data || count <= 0 || count > 256)
			{
				logging::console::print (xs ("[attrdump] bad vector count={} data={:#x}"), count, data);
				return;
			}

			struct target_entry { std::uint32_t hash; const char* name; };
			const target_entry targets []
			{
				{ 0x7b56e8e3u, "keychain slot 0 id" },
				{ 0x1b621ae1u, "keychain slot 0 seed" },
				{ 0xbc926961u, "keychain slot 0 offset x" },
				{ 0x0d5cbb6fu, "keychain slot 0 offset y" },
				{ 0x48a39fc4u, "keychain slot 0 offset z" },
				{ 0x3d48e1d9u, "sticker slot 0 id (guess)" },
			};

			constexpr auto scan_bytes {1024};

			char found[512] {};
			std::size_t foff {};

			__try
			{
				for (auto i = 0u; i + 4 <= scan_bytes && foff < sizeof (found) - 64; i += 4)
				{
					const auto key = memory::read<std::uint32_t> (data + i);
					for (const auto& t : targets)
					{
						if (key != t.hash)
						{
							continue;
						}

						const auto raw0 = memory::read<std::uint32_t> (data + i + 4);
						float fval {};
						std::memcpy (&fval, &raw0, sizeof (fval));
						foff += static_cast<std::size_t> (std::snprintf (found + foff, sizeof (found) - foff,
							"[+%u] %s = %.4f (%u); ", i, t.name, fval, raw0));
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				foff += static_cast<std::size_t> (std::snprintf (found + foff, sizeof (found) - foff, "<fault> "));
			}

			if (!foff)
			{
				std::snprintf (found, sizeof (found), "<none of our hashes present>");
			}

			logging::console::print (xs ("[attrdump] count={} data={:#x} hits: {}"), count, data, found);

			// Raw first elements - decode stride/key layout offline.
			char elem_hex[3 * 64] {};
			std::size_t eoff {};
			__try
			{
				for (auto i = 0u; i < 64 && eoff + 3 < sizeof (elem_hex); ++i)
				{
					const auto b = memory::read<std::uint8_t> (data + i);
					static constexpr char digits[] = "0123456789abcdef";
					elem_hex [eoff++] = digits [(b >> 4) & 0xF];
					elem_hex [eoff++] = digits [b & 0xF];
					elem_hex [eoff++] = ' ';
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
			elem_hex [eoff] = '\0';
			logging::console::print (xs ("[attrdump] first elems: {}"), elem_hex);

			// Raw bytes of the game's lazily-initialized attribute-name slots
			// used by the keychain attr packer sub_180750280 (id/x/y/z/seed/
			// highlight/sticker) plus the two id/x/y/z cache clusters used by
			// the spawn-side readers. Layout per slot: {char* name, void*
			// resolved_def, u32 schema_version}. Interpret offline.
			struct slot_entry { const char* tag; std::uint32_t rva; };
			constexpr slot_entry slots []
			{
				{ "pk_id",        0x2074438 },
				{ "pk_x",         0x2074450 },
				{ "pk_y",         0x2074468 },
				{ "pk_z",         0x2074480 },
				{ "pk_seed",      0x2074498 },
				{ "pk_highlight", 0x20744B0 },
				{ "pk_sticker",   0x20744C8 },
				{ "clA_id",       0x2075448 },
				{ "clA_x",        0x2075460 },
				{ "clA_y",        0x2075478 },
				{ "clA_z",        0x2075490 },
				{ "clB_id",       0x2075590 },
				{ "clB_x",        0x20755A8 },
				{ "clB_y",        0x20755C0 },
				{ "clB_z",        0x20755D8 },
			};

			const auto base = memory::get_module_base ("client.dll");

			for (const auto& s : slots)
			{
				char sa[96] {};
				const auto pa = memory::read<std::uintptr_t> (base + s.rva);
				const auto def = memory::read<std::uintptr_t> (base + s.rva + 8);
				try_read_string (pa, sa, sizeof (sa));
				logging::console::print (xs ("[attrslot] {} @ +{:x} name_ptr={:#x} '{}' def={:#x}"), s.tag, s.rva, pa, sa, def);
			}
		}

		// Murmur2-lower hashes of the attribute names we write, for matching
		// against dumped attribute keys.
		void dump_name_hashes ()
		{
			constexpr const char* names []
			{
				"keychain slot 0 id",
				"keychain slot 0 seed",
				"keychain slot 0 offset x",
				"keychain slot 0 offset y",
				"keychain slot 0 offset z",
			};

			for (const auto* n : names)
			{
				const auto h = murmurhash2_lower (n, static_cast<int> (std::strlen (n)), 0x31415926);
				logging::console::print (xs ("[attrhash] '{}' = {:#x}"), n, h);
			}
		}

} // namespace

	void apply_stattrak_attributes (std::uintptr_t item_view, bool enabled, int value)
	{
		if (!item_view)
		{
			return;
		}

		// Def 80 "kill eater" / def 81 "kill eater score type". Stored as a
		// float bit-cast, exactly like the reference implementations pass the
		// counter: ViewAsFloat(count). A bit-cast of -1 means "no stattrack".
		if (enabled)
		{
			set_attribute (item_view, "kill eater", bits_to_float (static_cast<std::uint32_t>(value)));
			set_attribute (item_view, "kill eater score type", bits_to_float (0));
			return;
		}

		set_attribute (item_view, "kill eater", bits_to_float (static_cast<std::uint32_t>(-1)));
		set_attribute (item_view, "kill eater score type", bits_to_float (static_cast<std::uint32_t>(-1)));
	}

	void add_nametag_entity (std::uintptr_t weapon, std::uintptr_t item_view)
	{
		const auto add = PATTERN (patterns::add_nametag_entity);
		if (add && weapon && item_view)
		{
			__try
			{
				memory::call<void> (add, weapon, item_view);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	void add_stattrak_entity (std::uintptr_t weapon, std::uintptr_t item_view)
	{
		const auto add = PATTERN (patterns::add_stattrak_entity);
		if (add && weapon && item_view)
		{
			__try
			{
				memory::call<void> (add, weapon, item_view);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	namespace {

		// Per-weapon keycharm sync state.
		//
		// Two game behaviours force this to be a state machine:
		//  1. UTIL_Remove is deferred - the module survives until the end of the
		//     frame, so remove+add in the same tick leaves the weapon's stored
		//     module handle still resolving to the dying entity.
		//  2. AddKeychainEntity early-outs WITHOUT spawning when that stored
		//     handle resolves and the module's defid+seed match the item view's
		//     attributes (offset-only edits match!). Combined with (1) an offset
		//     change would kill the charm permanently.
		// Therefore: remove this tick, add on a later tick once the old module
		// stopped resolving; and never re-add while a queued spawn has not yet
		// materialized in the scene tree.
		struct keychain_sync_state
		{
			settings::changer::keychain_slot requested {};
			bool valid {};            // a spawn was requested for this config
			bool pending {};          // spawn queued but not yet materialized
			bool removed {};          // removal issued, waiting for it to die
			std::chrono::steady_clock::time_point since {};
		};

		inline std::unordered_map<std::uint32_t, keychain_sync_state>& keychain_states ()
		{
			static std::unordered_map<std::uint32_t, keychain_sync_state> map;
			return map;
		}

		constexpr auto keychain_pending_timeout {std::chrono::milliseconds (750)};
		constexpr auto keychain_removed_timeout {std::chrono::milliseconds (150)};

		void remove_keychain_module (std::uintptr_t weapon)
		{
			const auto module = find_keychain_module (weapon);
			if (!module)
			{
				return;
			}

			const auto remove = PATTERN (patterns::remove_keychain_entity);
			if (!remove)
			{
				return;
			}

			__try
			{
				memory::call<void> (remove, module);
				logging::console::print (xs ("[kcdbg] sync: removed live module {:#x}"), module);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				logging::console::print (xs ("[kcdbg] sync: remove EXCEPTION"));
			}
		}

		// The charm placement code inside add_keychain_entity reads the offset
		// attributes through {name*, def*, schema_version} caches in .data
		// (rva 0x2074580/98/B0/C8). Those slots are lazily initialized by the
		// game and can still hold unrelated data when we spawn a module, which
		// makes every float lookup silently fail and drops our offsets.
		// Repair them directly: point the name slot at the canonical literal,
		// null the def and stamp a bogus version so the reader re-resolves it
		// with its own schema resolver on the next call.
		void refresh_keychain_attr_caches ()
		{
			struct attr_cache_entry
			{
				std::uint32_t name_rva;
				std::uint32_t slot_rva;
			};

			static constexpr attr_cache_entry entries []
			{
				{ 0x1A644A0, 0x2074580 }, // has-attribute check ("keychain slot 0 offset x")
				{ 0x1A644A0, 0x2074598 }, // offset x -> placement [0]
				{ 0x1A644C0, 0x20745B0 }, // offset y -> placement [1]
				{ 0x1A644E0, 0x20745C8 }, // offset z -> placement [2]
			};

			const auto base = memory::get_module_base ("client.dll");
			if (!base)
			{
				return;
			}

			auto repaired {0u};

			for (const auto& entry : entries)
			{
				const auto target_name = base + entry.name_rva;

				const auto current_name = memory::safe_read<std::uintptr_t> (base + entry.slot_rva);
				if (current_name && *current_name == target_name)
				{
					continue;
				}

				memory::write<std::uintptr_t> (base + entry.slot_rva, target_name);
				memory::write<std::uintptr_t> (base + entry.slot_rva + 8, 0);
				memory::write<std::uint32_t> (base + entry.slot_rva + 16, 0);

				++repaired;
			}

			if (repaired > 0)
			{
				logging::console::print (xs ("[kcdbg] repaired {} keychain attr cache slots"), repaired);
			}
		}

		void spawn_keychain_module (std::uintptr_t weapon, std::uintptr_t item_view)
		{
			const auto add = PATTERN (patterns::add_keychain_entity);
			if (!add)
			{
				return;
			}

			refresh_keychain_attr_caches ();

			__try
			{
				memory::call<void> (add, weapon, item_view, false);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
				logging::console::print (xs ("[kcdbg] sync: add EXCEPTION"));
			}
		}

	}

	// Idempotent charm sync - see changer.hpp declaration. Called from the FSN
	// applier for every owned gun/knife whose cosmetic config changed, plus on
	// first sight of the weapon.
	void sync_keychain (std::uint32_t weapon_handle, std::uintptr_t weapon, std::uintptr_t item_view, const settings::changer::keychain_slot& kc)
	{
		if (!weapon || !item_view)
		{
			return;
		}

		auto& state = keychain_states ()[weapon_handle];
		const auto now = std::chrono::steady_clock::now ();

		// A previously queued spawn never materialized (module destroyed by the
		// game, round restart, ...). Give up waiting so future updates work.
		if (state.pending && now - state.since > keychain_pending_timeout)
		{
			logging::console::print (xs ("[kcdbg] sync: pending spawn timed out h={}"), weapon_handle);
			state.pending = false;
		}

		const auto existing = find_keychain_module (weapon);

		// Phase 1 of a respawn: we removed the old module last tick. Wait until
		// it is really gone so add()'s stored-handle duplicate check cannot
		// early-out against the dying entity. Fallback timer keeps us unstuck if
		// the scene walk loses sight of it.
		if (state.removed && !existing)
		{
			state.removed = false;
		}
		else if (state.removed && now - state.since > keychain_removed_timeout)
		{
			logging::console::print (xs ("[kcdbg] sync: removed-wait timed out h={}"), weapon_handle);
			state.removed = false;
		}
		else if (state.removed)
		{
			return;
		}

		// While our last spawn is still materializing the scene walk cannot see
		// it - re-adding here would queue a duplicate. Wait until it shows up.
		if (state.pending)
		{
			if (existing)
			{
				logging::console::print (xs ("[kcdbg] sync: spawn materialized h={}"), weapon_handle);
				state.pending = false;
			}
			else
			{
				return;
			}
		}

		if (kc.id != 0)
		{
			// Live module already matches exactly what is configured: nothing to do.
			if (existing && state.valid && state.requested == kc)
			{
				return;
			}

			static std::atomic<int> dump_budget {3};
			if (dump_budget.load () > 0)
			{
				dump_budget.fetch_sub (1);
				dump_item_attributes (item_view);
			}

			logging::console::print (xs ("[kcdbg] sync: respawn h={} existing={:#x} id={} seed={} off=({:.2f},{:.2f},{:.2f})"), weapon_handle, existing, kc.id, kc.seed, kc.offset_x, kc.offset_y, kc.offset_z);

			state.since = now;

			if (existing)
			{
				// Remove first; the actual spawn happens on a later tick once
				// this module stopped resolving (see struct comment).
				remove_keychain_module (weapon);
				state.valid = false;
				state.removed = true;
				return;
			}

			spawn_keychain_module (weapon, item_view);
			state.requested = kc;
			state.valid = true;
			state.pending = true;
		}
		else
		{
			if (existing || state.valid || state.removed)
			{
				logging::console::print (xs ("[kcdbg] sync: clearing charm h={}"), weapon_handle);
				remove_keychain_module (weapon);
				state.removed = !existing;
				state.since = now;
			}
			else
			{
				state = {};
			}

			state.requested = {};
			state.valid = false;
			state.pending = false;
		}
	}

	void update_composite_material (std::uintptr_t weapon)
	{
		const auto update = PATTERN (patterns::weapon_update_composite_material);
		if (update && weapon)
		{
			__try
			{
				memory::call<void> (update, weapon, true);
			}
			__except (EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}

	void apply_sticker_keychain_attributes (std::uintptr_t item_view, const settings::changer::applied_skin& skin)
	{
		if (!item_view)
		{
			return;
		}

		logging::console::print (xs ("[keychain] apply_sticker_keychain_attributes item_view={:#x} stickers={} keychain_id={} set_attr_pat={:#x}"), item_view, skin.stickers [0].id, skin.keychain.id, PATTERN (patterns::econ_item_view_set_attribute));

		char name[64] {};

		for (auto slot = 0u; slot < sticker_slot_count; ++slot)
		{
			const auto& st = skin.stickers [slot];

			std::snprintf (name, sizeof (name), "sticker slot %u id", slot);

			if (st.id == 0)
			{
				set_attribute (item_view, name, 0.0f);
				continue;
			}

			set_int_attribute (item_view, name, st.id);

			std::snprintf (name, sizeof (name), "sticker slot %u schema", slot);
			set_int_attribute (item_view, name, st.schema);

			std::snprintf (name, sizeof (name), "sticker slot %u wear", slot);
			set_attribute (item_view, name, st.wear);

			std::snprintf (name, sizeof (name), "sticker slot %u scale", slot);
			set_attribute (item_view, name, st.scale);

			std::snprintf (name, sizeof (name), "sticker slot %u rotation", slot);
			set_attribute (item_view, name, st.rotation);

			std::snprintf (name, sizeof (name), "sticker slot %u offset x", slot);
			set_attribute (item_view, name, st.offset_x);

			std::snprintf (name, sizeof (name), "sticker slot %u offset y", slot);
			set_attribute (item_view, name, st.offset_y);
		}

		logging::console::print (xs ("[keychain]   sticker attrs written"));

		if (skin.keychain.id == 0)
		{
			set_attribute (item_view, "keychain slot 0 id", 0.0f);
			logging::console::print (xs ("[keychain]   keychain slot cleared (id=0)"));
			return;
		}

		set_int_attribute (item_view, "keychain slot 0 id", skin.keychain.id);
		set_int_attribute (item_view, "keychain slot 0 seed", skin.keychain.seed);
		set_attribute (item_view, "keychain slot 0 offset x", skin.keychain.offset_x);
		set_attribute (item_view, "keychain slot 0 offset y", skin.keychain.offset_y);
		set_attribute (item_view, "keychain slot 0 offset z", skin.keychain.offset_z);
		logging::console::print (xs ("[keychain]   keychain attrs written id={} seed={} off=({},{},{})"), skin.keychain.id, skin.keychain.seed, skin.keychain.offset_x, skin.keychain.offset_y, skin.keychain.offset_z);
	}

	void clear_sticker_keychain_attributes (std::uintptr_t item_view)
	{
		if (!item_view)
		{
			return;
		}

		char name[64] {};

		for (auto slot = 0u; slot < sticker_slot_count; ++slot)
		{
			std::snprintf (name, sizeof (name), "sticker slot %u id", slot);
			set_attribute (item_view, name, 0.0f);
		}

		set_attribute (item_view, "keychain slot 0 id", 0.0f);
	}

} // namespace features::changer::detail
