#include <pch/pch.hpp>
#include <utilities/diag.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/hooking/hooking.hpp>
#include <utilities/logging/logging.hpp>
#include <core/systems/systems.hpp>
#include <core/settings.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>
#include <cstring>
#include <unordered_map>
#include "../hooks.hpp"

namespace {

	namespace custom_paint {

		void debug_log( const char* format, ... )
		{
			char buffer[ 2048 ];
			va_list args;
			va_start( args, format );
			vsnprintf_s( buffer, _TRUNCATE, format, args );
			va_end( args );
			diag::write( diag::level::debug, buffer );
		}

		enum { LOOSE_VAR_COLOR4 = 9 };

		struct CompositeMaterialInputLooseVariable_t
		{
			char* m_strName; // CUtlString
			std::uint8_t pad_008[ 0x38 ];
			std::int32_t m_nVariableType;
			std::uint8_t pad_044[ 0x48 ];
			std::uint32_t m_cValueColor4; // rgba
			std::uint8_t pad_090[ 0x288 - 0x90 ];
		};

		static_assert( offsetof( CompositeMaterialInputLooseVariable_t, m_nVariableType ) == 0x40 );
		static_assert( offsetof( CompositeMaterialInputLooseVariable_t, m_cValueColor4 ) == 0x8C );
		static_assert( sizeof( CompositeMaterialInputLooseVariable_t ) == 0x288 );

		using append_fn = void( __fastcall* )( void*, const CompositeMaterialInputLooseVariable_t* );

		// E8 ? ? ? ? 0F 28 B4 24 ? ? ? ? 4C 39 A5 @ client.dll (call target)
		append_fn g_append{};

		char* tier0_dup( const char* s )
		{
			using alloc_fn = void* ( __fastcall* )( std::size_t );
			static auto alloc = reinterpret_cast< alloc_fn >( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "MemAlloc_AllocFunc" ) );
			if ( !s || !alloc )
			{
				return nullptr;
			}

			const auto len = std::strlen( s ) + 1;
			auto* p = static_cast< char* >( alloc( len ) );
			if ( !p )
			{
				return nullptr;
			}

			std::memcpy( p, s, len );
			return p;
		}

		std::uint16_t read_weapon_def_index( void* weapon )
		{
			if ( !weapon )
			{
				return 0;
			}

			__try
			{
				const auto attr_mgr = reinterpret_cast< std::uintptr_t >( weapon ) + SCHEMA( "C_EconEntity", "m_AttributeManager"_hash );
				const auto item = attr_mgr + SCHEMA( "C_AttributeContainer", "m_Item"_hash );
				return memory::read< std::uint16_t >( item + SCHEMA( "C_EconItemView", "m_iItemDefinitionIndex"_hash ) );
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return 0;
			}
		}

		thread_local std::uint16_t g_weapon_material_def_index = 0;
		thread_local int g_weapon_material_depth = 0;

		struct raw_vector_t
		{
			void* elements;
			int size;
			int capacity;
		};

		struct vector_state_t
		{
			bool weapon_colors_appended = false;
			std::uint16_t weapon_def_index = 0;
		};

		// Per-thread: material building can run on multiple threads (e.g. inventory
// preview) while gameplay builds on the main thread. A shared map mutated
// concurrently is a data race (node-walk AV); TLS keeps it lock-free and safe.
thread_local std::unordered_map< void*, vector_state_t > g_vector_states;

		std::uint16_t push_weapon_context( void* weapon )
		{
			const auto previous = g_weapon_material_def_index;
			g_weapon_material_def_index = read_weapon_def_index( weapon );
			++g_weapon_material_depth;
			return previous;
		}

		std::uint16_t current_weapon_context_def_index( )
		{
			return g_weapon_material_depth > 0 ? g_weapon_material_def_index : 0;
		}

		void pop_weapon_context( std::uint16_t previous_def_index )
		{
			if ( g_weapon_material_depth > 0 )
			{
				--g_weapon_material_depth;
			}

			g_weapon_material_def_index = previous_def_index;
		}

		const char* safe_name( const CompositeMaterialInputLooseVariable_t* var )
		{
			if ( !var )
			{
				return nullptr;
			}

			__try
			{
				return var->m_strName;
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return nullptr;
			}
		}

		bool safe_equals( const char* lhs, const char* rhs )
		{
			if ( !lhs || !rhs )
			{
				return false;
			}

			__try
			{
				return std::strcmp( lhs, rhs ) == 0;
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return false;
			}
		}

		int color_trigger_index( const char* name )
		{
			if ( !name )
			{
				return -1;
			}

			__try
			{
				if ( std::strncmp( name, "g_vColor", 8 ) != 0 )
				{
					return -1;
				}

				const auto index_char = name[ 8 ];
				if ( index_char < '0' || index_char > '3' || name[ 9 ] != '\0' )
				{
					return -1;
				}

				return index_char - '0';
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				return -1;
			}
		}

		void append_color_var( void* vector, append_fn append, const char* name, std::uint32_t color )
		{
			if ( !vector || !append || !name )
			{
				return;
			}

			CompositeMaterialInputLooseVariable_t var{};
			var.m_strName = tier0_dup( name ); // game moves ptr
			if ( !var.m_strName )
			{
				return;
			}

			var.m_nVariableType = LOOSE_VAR_COLOR4;
			var.m_cValueColor4 = color;
			append( vector, &var );
		}

		const xdraw::color* weapon_colors_for_def_index( std::uint16_t def_index, int& count )
		{
			count = 0;

			if ( !def_index )
			{
				return nullptr;
			}

			// Snapshot under the same mutex the menu uses for its writes; the
			// caller keeps using these colors after we unlock, so a map entry
			// being erased mid-edit must not leave a dangling paint_colors view.
			static thread_local xdraw::color stash[ 4 ];

			{
				std::lock_guard<std::recursive_mutex> lock{ settings::g_changer.skins.mx };

				const auto& data = settings::g_changer.skins.data;
				const auto it = data.find( static_cast< std::int16_t >( def_index ) );
				if ( it == data.end( ) || !it->second.paint_color )
				{
					return nullptr;
				}

				std::memcpy( stash, it->second.paint_colors, sizeof( stash ) );
			}

			count = 4;
			return stash;
		}

		void append_weapon_colors( void* vector, append_fn append, const xdraw::color* colors, int count )
		{
			for ( auto i = 0; i < count; ++i )
			{
				char name[ 16 ];
				std::snprintf( name, sizeof( name ), "g_vColor%d", i );
				append_color_var( vector, append, name, colors[ i ].val );
			}
		}

		// Pastehook-parity: append the config color set only ONCE per material
		// vector; without this guard every g_nRandomSeed trigger (multiple per
		// material rebuild, one material per weapon) would keep appending and
		// the loose-variable vector would grow unboundedly until crash.
		std::uint16_t active_weapon_paint_def_index( )
		{
			int count = 0;
			const auto def_index = current_weapon_context_def_index( );
			if ( weapon_colors_for_def_index( def_index, count ) )
			{
				return def_index;
			}

			return 0;
		}

		bool is_glove_def( std::uint16_t def_index )
		{
			const auto* def = features::changer::g_econ_item_system.find_def( static_cast< std::int16_t >( def_index ) );
			return def && def->category == features::changer::econ_item_system::item_category::glove;
		}

		void maybe_append_config_colors( void* vector, append_fn append, vector_state_t& state )
		{
			if ( state.weapon_colors_appended )
			{
				return;
			}

			const auto current_def_index = active_weapon_paint_def_index( );
			if ( current_def_index == 0 )
			{
				state.weapon_def_index = 0;
				state.weapon_colors_appended = false;
				return;
			}

			if ( state.weapon_def_index != current_def_index )
			{
				state.weapon_def_index = current_def_index;
				state.weapon_colors_appended = false;
			}

			int color_count = 0;
			const auto* colors = weapon_colors_for_def_index( state.weapon_def_index, color_count );
			if ( !colors || color_count <= 0 )
			{
				return;
			}

			append_weapon_colors( vector, append, colors, color_count );
			state.weapon_colors_appended = true;

			debug_log( "paint.material colors applied def_index=%u", state.weapon_def_index );
		}

		void append_loose_variable( void* vector, const CompositeMaterialInputLooseVariable_t* input, append_fn append )
		{
			if ( !vector || !input || !append )
			{
				return;
			}

			__try
			{
				auto* raw_vector = reinterpret_cast< raw_vector_t* >( vector );
				if ( raw_vector->size <= 0 )
				{
					const auto previous = g_vector_states[ vector ];
					g_vector_states[ vector ] = {};

					if ( g_vector_states.size( ) > 256 )
					{
						g_vector_states.clear( );
					}

					debug_log( "paint.vector reset v=0x%p prevsize=%d prevdef=%u prevapp=%d states=%zu", vector, raw_vector->size, previous.weapon_def_index, previous.weapon_colors_appended, g_vector_states.size( ) );
				}

				auto& state = g_vector_states[ vector ];
				const auto* name = safe_name( input );

				if ( safe_equals( name, "g_nRandomSeedAlt" ) )
				{
					debug_log( "paint.seedAlt v=0x%p ctxdef=%u depth=%d stdef=%u stapp=%d -> append set", vector, g_weapon_material_def_index, g_weapon_material_depth, state.weapon_def_index, state.weapon_colors_appended );
					append( vector, input );
					maybe_append_config_colors( vector, append, state );
					return;
				}

				const auto color_index = color_trigger_index( name );

				// Glove materials tint through a single g_vColorTint loose
				// variable instead of the weapon-style g_vColor0..3 set.
				// Swap it for the configured paint colour when the material
				// being built belongs to a gloved item with paint colour on.
				if ( safe_equals( name, "g_vColorTint" ) )
				{
					const auto def_index = current_weapon_context_def_index( );
					int glove_color_count = 0;

					if ( def_index && is_glove_def( def_index ) )
					{
						const auto* tint_colors = weapon_colors_for_def_index( def_index, glove_color_count );
						if ( tint_colors && glove_color_count > 0 )
						{
							debug_log( "paint.gloveTintReplace def=%u -> 0x%08X", def_index, tint_colors[ 0 ].val );
							auto replaced = *input;
							replaced.m_nVariableType = LOOSE_VAR_COLOR4;
							replaced.m_cValueColor4 = tint_colors[ 0 ].val;
							append( vector, &replaced );
							return;
						}
					}

					append( vector, input );
					return;
				}

				if ( safe_equals( name, "g_nRandomSeed" ) )
				{
					debug_log( "paint.seed v=0x%p ctxdef=%u depth=%d stdef=%u stapp=%d", vector, g_weapon_material_def_index, g_weapon_material_depth, state.weapon_def_index, state.weapon_colors_appended );
					append( vector, input );
					if ( state.weapon_def_index == 0 )
					{
						state.weapon_def_index = active_weapon_paint_def_index( );
					}

					maybe_append_config_colors( vector, append, state );
					return;
				}

				if ( color_index < 0 )
				{
					append( vector, input );
					return;
				}

				int color_count = 0;
				const auto current_def_index = active_weapon_paint_def_index( );
				if ( current_def_index == 0 )
				{
					debug_log( "paint.colorFwd idx=%d nocontext v=0x%p", color_index, vector );
					append( vector, input );
					return;
				}

				if ( state.weapon_def_index != current_def_index )
				{
					debug_log( "paint.defSwitch v=0x%p old=%u new=%u", vector, state.weapon_def_index, current_def_index );
					state.weapon_def_index = current_def_index;
					state.weapon_colors_appended = false;
				}

				const auto* colors = weapon_colors_for_def_index( state.weapon_def_index, color_count );
				if ( !colors || color_index >= color_count )
				{
					append( vector, input );
					return;
				}

				debug_log( "paint.colorReplace idx=%d def=%u -> 0x%08X", color_index, state.weapon_def_index, colors[ color_index ].val );
				auto replaced = *input;
				replaced.m_nVariableType = LOOSE_VAR_COLOR4;
				replaced.m_cValueColor4 = colors[ color_index ].val;
				append( vector, &replaced );
				maybe_append_config_colors( vector, append, state );
			}
			__except ( EXCEPTION_EXECUTE_HANDLER )
			{
				debug_log( "paint.exception caught in append_loose_variable v=0x%p", vector );
				append( vector, input );
			}
		}

	} // namespace custom_paint

} // namespace

namespace hooks {

	bool skin_paint::initialize( )
	{
		custom_paint::g_append = reinterpret_cast< custom_paint::append_fn >( PATTERN( patterns::composite_material_add_to_tail ) );
		custom_paint::debug_log( "paint.initialize g_append=0x%p", reinterpret_cast< const void* >( custom_paint::g_append ) );
		custom_paint::debug_log( "paint.initialize build_legacy=0x%p", reinterpret_cast< void* >( PATTERN( patterns::build_legacy_weapon_skin_material ) ) );
		custom_paint::debug_log( "paint.initialize build_modern=0x%p", reinterpret_cast< void* >( PATTERN( patterns::build_modern_weapon_skin_material ) ) );

		if ( !custom_paint::g_append )
		{
			logging::console::print( xs( "skipping custom paint: composite material add-to-tail not resolved" ) );
			return true;
		}

		if ( !hooking::manager::create( {
			{ &m_build_legacy, &build_legacy_weapon_skin_material, xs( "build_legacy_weapon_skin_material" ), PATTERN( patterns::build_legacy_weapon_skin_material ) },
			{ &m_build_modern, &build_modern_weapon_skin_material, xs( "build_modern_weapon_skin_material" ), PATTERN( patterns::build_modern_weapon_skin_material ) },
			{ &m_add_to_tail, &add_to_tail, xs( "composite_material_add_to_tail" ), reinterpret_cast< std::uintptr_t >( custom_paint::g_append ) }
		} ) )
		{
			logging::console::print( xs( "skipping custom paint: material hooks failed" ) );
			return true;
		}

		logging::console::print( xs( "custom paint hooks installed" ) );
		return true;
	}

	void skin_paint::shutdown( )
	{
		m_build_legacy.reset( );
		m_build_modern.reset( );
		m_add_to_tail.reset( );
	}

	void __fastcall skin_paint::build_legacy_weapon_skin_material( void* weapon, bool force )
	{
		const auto previous = custom_paint::push_weapon_context( weapon );
		custom_paint::debug_log( "paint.build legacy in weapon=0x%p def=%u depth=%d", weapon, custom_paint::g_weapon_material_def_index, custom_paint::g_weapon_material_depth );

		__try
		{
			m_build_legacy.call< void >( weapon, force );
		}
		__finally
		{
			custom_paint::pop_weapon_context( previous );
			custom_paint::debug_log( "paint.build legacy out def=%u depth=%d", custom_paint::g_weapon_material_def_index, custom_paint::g_weapon_material_depth );
		}
	}

	void __fastcall skin_paint::build_modern_weapon_skin_material( void* weapon, void* a2, void* a3, int a4, char a5, char a6, void* a7 )
	{
		const auto previous = custom_paint::push_weapon_context( weapon );
		custom_paint::debug_log( "paint.build modern in weapon=0x%p def=%u depth=%d", weapon, custom_paint::g_weapon_material_def_index, custom_paint::g_weapon_material_depth );

		__try
		{
			m_build_modern.call< void >( weapon, a2, a3, a4, a5, a6, a7 );
		}
		__finally
		{
			custom_paint::pop_weapon_context( previous );
			custom_paint::debug_log( "paint.build modern out def=%u depth=%d", custom_paint::g_weapon_material_def_index, custom_paint::g_weapon_material_depth );
		}
	}

	void __fastcall skin_paint::add_to_tail( void* vector, const void* input )
	{
		const auto append = m_add_to_tail.original< custom_paint::append_fn >( );
		if ( !append )
		{
			return;
		}

		custom_paint::append_loose_variable(
			vector,
			reinterpret_cast< const custom_paint::CompositeMaterialInputLooseVariable_t* >( input ),
			append );
	}

} // namespace hooks