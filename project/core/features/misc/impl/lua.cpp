#include <pch/pch.hpp>
#include <core/features/features.hpp>
#include <core/systems/systems.hpp>
#include <external/xdraw/xdraw.hpp>
#include <utilities/game_path.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/steam/steam.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/addresses/interfaces.hpp>
#include <utilities/fnv1a.hpp>
#include <utilities/cstypes.hpp>
#include <protection/game_addresses.hpp>

#include <cstring>
#include <cmath>
#include <unordered_map>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

namespace features::misc {

	namespace {


		struct paint_state
		{
			xdraw::draw_list* draw_list{};
			std::uintptr_t pawn{};
		};

		auto& current_paint( )
		{
			static paint_state state{};
			return state;
		}


		constexpr auto k_vector_mt = "aetheris_vector";
		constexpr auto k_color_mt = "aetheris_color";
		constexpr auto k_cvar_mt = "aetheris_cvar";
		constexpr auto k_entity_mt = "aetheris_entity";
		constexpr auto k_event_mt = "aetheris_event";

		struct vec_data
		{
			float x{}, y{}, z{};
		};

		struct col_data
		{
			float r{}, g{}, b{}, a{};
		};

		struct cvar_data
		{
			std::uintptr_t ptr{};
			char name[ 64 ]{};
		};

		struct ent_data
		{
			std::uintptr_t pawn{};
			std::uintptr_t controller{};
			std::int16_t index{ -1 };
		};

		struct event_data
		{
			char name[ 64 ]{};
		};

		inline std::unordered_map<std::string, std::vector<int>> g_event_refs{};


		[[nodiscard]] std::int32_t prop_offset( const char* class_name, std::uint32_t field_hash )
		{
			static std::unordered_map<std::uint64_t, std::int32_t> cache{};

			const auto key =
				( static_cast< std::uint64_t >( fnv1a::runtime_hash( class_name ) ) << 32u ) | field_hash;
			const auto it = cache.find( key );
			if ( it != cache.end( ) )
			{
				return it->second;
			}

			const auto offset = static_cast< std::int32_t >( systems::schemas::lookup( class_name, field_hash ) );
			cache.emplace( key, offset );
			return offset;
		}

		enum class prop_kind : std::uint8_t
		{
			integer,
			boolean,
			floating,
			vector,
			string,
			int64
		};

		[[nodiscard]] prop_kind prop_kind_from_name( const std::string& name )
		{
			if ( name.rfind( "m_b", 0 ) == 0 ) return prop_kind::boolean;
			if ( name.rfind( "m_fl", 0 ) == 0 ) return prop_kind::floating;
			if ( name.rfind( "m_vec", 0 ) == 0 || name.rfind( "m_v", 0 ) == 0 || name.rfind( "m_ang", 0 ) == 0 ) return prop_kind::vector;
			if ( name.rfind( "m_ul", 0 ) == 0 ) return prop_kind::int64;
			if ( name.rfind( "m_s", 0 ) == 0 ) return prop_kind::string;
			return prop_kind::integer;
		}

		struct prop_result
		{
			std::uintptr_t base{};
			std::int32_t offset{ -1 };
			bool found{};
		};

		[[nodiscard]] std::uintptr_t prop_offset_cached( const char* class_name, std::uint32_t field_hash )
		{
			return prop_offset( class_name, field_hash );
		}

		[[nodiscard]] prop_result resolve_entity_prop( const ent_data& e, const std::string& name )
		{
			prop_result out{};
			const auto field_hash = fnv1a::runtime_hash( name.c_str( ) );
			if ( !field_hash )
			{
				return out;
			}

			const auto kind = prop_kind_from_name( name );

			static const char* k_pawn_classes[] =
			{
				"CCSPlayerPawn",
				"CCSPlayerPawnBase",
				"C_CSPlayerPawnBase",
				"C_CSPlayerPawn",
				"C_BasePlayerPawn",
				"CBasePlayerPawn",
				"C_BaseEntity",
			};

			static const char* k_ctrl_classes[] =
			{
				"CCSPlayerController",
				"CBasePlayerController",
			};

			if ( e.pawn )
			{
				for ( const auto* cls : k_pawn_classes )
				{
					const auto off = prop_offset( cls, field_hash );
					if ( off > 0 )
					{
						out.base = e.pawn;
						out.offset = off;
						out.found = true;
						return out;
					}
				}
			}

			if ( e.controller )
			{
				for ( const auto* cls : k_ctrl_classes )
				{
					const auto off = prop_offset( cls, field_hash );
					if ( off > 0 )
					{
						out.base = e.controller;
						out.offset = off;
						out.found = true;
						return out;
					}
				}
			}

			return out;
		}


		[[nodiscard]] std::string arg_to_string( lua_State* L, int idx )
		{
			switch ( lua_type( L, idx ) )
			{
			case LUA_TSTRING:
				return lua_tostring( L, idx );
			case LUA_TNUMBER:
				return std::to_string( lua_tonumber( L, idx ) );
			case LUA_TBOOLEAN:
				return lua_toboolean( L, idx ) ? "true" : "false";
			default:
				return {};
			}
		}

		[[nodiscard]] std::uint8_t clamp_channel( float v )
		{
			return static_cast< std::uint8_t >( std::clamp( v, 0.0f, 255.0f ) );
		}

		[[nodiscard]] bool read_pos( lua_State* L, int idx, float& x, float& y, int& consumed )
		{
			if ( lua_type( L, idx ) == LUA_TUSERDATA )
			{
				if ( const auto* v = static_cast< vec_data* >( luaL_testudata( L, idx, k_vector_mt ) ) )
				{
					x = v->x;
					y = v->y;
					consumed = 1;
					return true;
				}
			}

			x = static_cast< float >( luaL_checknumber( L, idx ) );
			y = static_cast< float >( luaL_checknumber( L, idx + 1 ) );
			consumed = 2;
			return true;
		}

		[[nodiscard]] xdraw::color read_color( lua_State* L, int idx, int& consumed )
		{
			if ( lua_type( L, idx ) == LUA_TUSERDATA )
			{
				if ( const auto* c = static_cast< col_data* >( luaL_testudata( L, idx, k_color_mt ) ) )
				{
					consumed = 1;
					return xdraw::color{ clamp_channel( c->r ), clamp_channel( c->g ), clamp_channel( c->b ), clamp_channel( c->a ) };
				}
			}

			consumed = 1;
			return xdraw::color{
				clamp_channel( static_cast< float >( luaL_optnumber( L, idx, 255 ) ) ),
				clamp_channel( static_cast< float >( luaL_optnumber( L, idx + 1, 255 ) ) ),
				clamp_channel( static_cast< float >( luaL_optnumber( L, idx + 2, 255 ) ) ),
				clamp_channel( static_cast< float >( luaL_optnumber( L, idx + 3, 255 ) ) ),
			};
		}


		[[nodiscard]] vec_data* push_vector( lua_State* L, float x, float y, float z = 0.0f )
		{
			auto* v = static_cast< vec_data* >( lua_newuserdatauv( L, sizeof( vec_data ), 0 ) );
			v->x = x;
			v->y = y;
			v->z = z;
			luaL_setmetatable( L, k_vector_mt );
			return v;
		}

		[[nodiscard]] col_data* push_color( lua_State* L, float r, float g, float b, float a = 255.0f )
		{
			auto* c = static_cast< col_data* >( lua_newuserdatauv( L, sizeof( col_data ), 0 ) );
			c->r = r;
			c->g = g;
			c->b = b;
			c->a = a;
			luaL_setmetatable( L, k_color_mt );
			return c;
		}

		[[nodiscard]] cvar_data* push_cvar( lua_State* L, std::uintptr_t ptr, const char* name )
		{
			auto* c = static_cast< cvar_data* >( lua_newuserdatauv( L, sizeof( cvar_data ), 0 ) );
			c->ptr = ptr;
			if ( name )
			{
				std::strncpy( c->name, name, sizeof( c->name ) - 1 );
			}
			luaL_setmetatable( L, k_cvar_mt );
			return c;
		}

		[[nodiscard]] ent_data* push_entity( lua_State* L, std::uintptr_t pawn, std::uintptr_t controller, std::int16_t index )
		{
			auto* e = static_cast< ent_data* >( lua_newuserdatauv( L, sizeof( ent_data ), 0 ) );
			e->pawn = pawn;
			e->controller = controller;
			e->index = index;
			luaL_setmetatable( L, k_entity_mt );
			return e;
		}

		[[nodiscard]] event_data* push_event( lua_State* L, const char* name )
		{
			auto* e = static_cast< event_data* >( lua_newuserdatauv( L, sizeof( event_data ), 0 ) );
			if ( name )
			{
				std::strncpy( e->name, name, sizeof( e->name ) - 1 );
			}
			luaL_setmetatable( L, k_event_mt );
			return e;
		}


		[[nodiscard]] vec_data* check_vector( lua_State* L, int idx )
		{
			return static_cast< vec_data* >( luaL_checkudata( L, idx, k_vector_mt ) );
		}

		[[nodiscard]] col_data* check_color( lua_State* L, int idx )
		{
			return static_cast< col_data* >( luaL_checkudata( L, idx, k_color_mt ) );
		}

		[[nodiscard]] cvar_data* check_cvar( lua_State* L, int idx )
		{
			return static_cast< cvar_data* >( luaL_checkudata( L, idx, k_cvar_mt ) );
		}

		[[nodiscard]] ent_data* check_entity( lua_State* L, int idx )
		{
			return static_cast< ent_data* >( luaL_checkudata( L, idx, k_entity_mt ) );
		}

		[[nodiscard]] event_data* check_event( lua_State* L, int idx )
		{
			return static_cast< event_data* >( luaL_checkudata( L, idx, k_event_mt ) );
		}


		int l_vector_index( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			if ( lua_type( L, 2 ) == LUA_TSTRING )
			{
				const auto* k = lua_tostring( L, 2 );
				if ( k && !k[ 1 ] )
				{
					switch ( *k )
					{
					case 'x': lua_pushnumber( L, v->x ); return 1;
					case 'y': lua_pushnumber( L, v->y ); return 1;
					case 'z': lua_pushnumber( L, v->z ); return 1;
					default: break;
					}
				}
			}

			if ( lua_type( L, 2 ) == LUA_TNUMBER )
			{
				switch ( static_cast< int >( lua_tointeger( L, 2 ) ) )
				{
				case 0: lua_pushnumber( L, v->x ); return 1;
				case 1: lua_pushnumber( L, v->y ); return 1;
				case 2: lua_pushnumber( L, v->z ); return 1;
				default: break;
				}
			}

			lua_pushvalue( L, lua_upvalueindex( 1 ) );
			lua_pushvalue( L, 2 );
			lua_gettable( L, -2 );
			return 1;
		}

		int l_vector_newindex( lua_State* L )
		{
			auto* v = check_vector( L, 1 );
			if ( lua_type( L, 2 ) == LUA_TSTRING )
			{
				const auto* k = lua_tostring( L, 2 );
				if ( k && !k[ 1 ] )
				{
					const auto value = static_cast< float >( luaL_checknumber( L, 3 ) );
					switch ( *k )
					{
					case 'x': v->x = value; return 0;
					case 'y': v->y = value; return 0;
					case 'z': v->z = value; return 0;
					default: break;
					}
				}
			}

			return luaL_error( L, "vector: cannot set field '%s'", lua_tostring( L, 2 ) ? lua_tostring( L, 2 ) : "?" );
		}

		int l_vector_length( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			lua_pushnumber( L, std::sqrt( v->x * v->x + v->y * v->y + v->z * v->z ) );
			return 1;
		}

		int l_vector_length2d( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			lua_pushnumber( L, std::sqrt( v->x * v->x + v->y * v->y ) );
			return 1;
		}

		int l_vector_length_sqr( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			lua_pushnumber( L, v->x * v->x + v->y * v->y + v->z * v->z );
			return 1;
		}

		int l_vector_copy( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			push_vector( L, v->x, v->y, v->z );
			return 1;
		}

		int l_vector_set( lua_State* L )
		{
			auto* v = check_vector( L, 1 );
			v->x = static_cast< float >( luaL_checknumber( L, 2 ) );
			v->y = static_cast< float >( luaL_checknumber( L, 3 ) );
			v->z = static_cast< float >( luaL_optnumber( L, 4, 0.0f ) );
			lua_settop( L, 1 );
			return 1;
		}

		int l_vector_to_screen( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			if ( !systems::g_view.has_camera( ) )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto screen = systems::g_view.project( math::vector3{ v->x, v->y, v->z } );
			if ( screen.x == 57005.0f || screen.y == 57005.0f )
			{
				lua_pushnil( L );
				return 1;
			}

			push_vector( L, screen.x, screen.y );
			return 1;
		}

		int l_vector_add( lua_State* L )
		{
			const auto* a = check_vector( L, 1 );
			const auto* b = check_vector( L, 2 );
			push_vector( L, a->x + b->x, a->y + b->y, a->z + b->z );
			return 1;
		}

		int l_vector_sub( lua_State* L )
		{
			const auto* a = check_vector( L, 1 );
			const auto* b = check_vector( L, 2 );
			push_vector( L, a->x - b->x, a->y - b->y, a->z - b->z );
			return 1;
		}

		int l_vector_mul( lua_State* L )
		{
			const auto* a = check_vector( L, 1 );
			if ( lua_type( L, 2 ) == LUA_TUSERDATA )
			{
				const auto* b = check_vector( L, 2 );
				push_vector( L, a->x * b->x, a->y * b->y, a->z * b->z );
			}
			else
			{
				const auto s = static_cast< float >( luaL_checknumber( L, 2 ) );
				push_vector( L, a->x * s, a->y * s, a->z * s );
			}
			return 1;
		}

		int l_vector_div( lua_State* L )
		{
			const auto* a = check_vector( L, 1 );
			const auto s = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto inv = s != 0.0f ? 1.0f / s : 0.0f;
			push_vector( L, a->x * inv, a->y * inv, a->z * inv );
			return 1;
		}

		int l_vector_unm( lua_State* L )
		{
			const auto* a = check_vector( L, 1 );
			push_vector( L, -a->x, -a->y, -a->z );
			return 1;
		}

		int l_vector_eq( lua_State* L )
		{
			const auto* a = check_vector( L, 1 );
			const auto* b = check_vector( L, 2 );
			lua_pushboolean( L, a->x == b->x && a->y == b->y && a->z == b->z );
			return 1;
		}

		int l_vector_tostring( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			char buf[ 96 ]{};
			std::snprintf( buf, sizeof( buf ), "(%.3f, %.3f, %.3f)", v->x, v->y, v->z );
			lua_pushstring( L, buf );
			return 1;
		}

		int l_vector_call( lua_State* L )
		{
			const auto argc = lua_gettop( L );
			float x = 0.0f, y = 0.0f, z = 0.0f;
			if ( argc >= 1 && lua_type( L, 1 ) == LUA_TUSERDATA )
			{
				const auto* src = check_vector( L, 1 );
				x = src->x;
				y = src->y;
				z = src->z;
			}
			else
			{
				x = static_cast< float >( luaL_optnumber( L, 1, 0.0f ) );
				y = static_cast< float >( luaL_optnumber( L, 2, 0.0f ) );
				z = static_cast< float >( luaL_optnumber( L, 3, 0.0f ) );
			}
			push_vector( L, x, y, z );
			return 1;
		}


		int l_color_index( lua_State* L )
		{
			const auto* c = check_color( L, 1 );
			if ( lua_type( L, 2 ) == LUA_TSTRING )
			{
				const auto* k = lua_tostring( L, 2 );
				if ( k && !k[ 1 ] )
				{
					switch ( *k )
					{
					case 'r': lua_pushnumber( L, c->r ); return 1;
					case 'g': lua_pushnumber( L, c->g ); return 1;
					case 'b': lua_pushnumber( L, c->b ); return 1;
					case 'a': lua_pushnumber( L, c->a ); return 1;
					default: break;
					}
				}
			}
			lua_pushnil( L );
			return 1;
		}

		int l_color_newindex( lua_State* L )
		{
			auto* c = check_color( L, 1 );
			if ( lua_type( L, 2 ) == LUA_TSTRING )
			{
				const auto* k = lua_tostring( L, 2 );
				if ( k && !k[ 1 ] )
				{
					const auto value = static_cast< float >( luaL_checknumber( L, 3 ) );
					switch ( *k )
					{
					case 'r': c->r = value; return 0;
					case 'g': c->g = value; return 0;
					case 'b': c->b = value; return 0;
					case 'a': c->a = value; return 0;
					default: break;
					}
				}
			}
			return 0;
		}

		int l_color_tostring( lua_State* L )
		{
			const auto* c = check_color( L, 1 );
			char buf[ 64 ]{};
			std::snprintf( buf, sizeof( buf ), "color(%d, %d, %d, %d)",
				static_cast< int >( c->r ), static_cast< int >( c->g ), static_cast< int >( c->b ), static_cast< int >( c->a ) );
			lua_pushstring( L, buf );
			return 1;
		}

		int l_color_eq( lua_State* L )
		{
			const auto* a = check_color( L, 1 );
			const auto* b = check_color( L, 2 );
			lua_pushboolean( L, a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a );
			return 1;
		}

		int l_color_call( lua_State* L )
		{
			auto r = static_cast< float >( luaL_optnumber( L, 1, 255.0f ) );
			auto g = static_cast< float >( luaL_optnumber( L, 2, 255.0f ) );
			auto b = static_cast< float >( luaL_optnumber( L, 3, 255.0f ) );
			auto a = static_cast< float >( luaL_optnumber( L, 4, 255.0f ) );

			if ( a > 0.0f && a <= 1.0f )
			{
				a *= 255.0f;
			}

			push_color( L, r, g, b, a );
			return 1;
		}


		int l_globals_index( lua_State* L )
		{
			if ( lua_type( L, 2 ) != LUA_TSTRING )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto* key = lua_tostring( L, 2 );
			const auto gv = addresses::globals::global_vars
				? memory::safe_read<std::uintptr_t>( addresses::globals::global_vars ).value_or( 0 )
				: std::uintptr_t{};

			if ( std::strcmp( key, "curtime" ) == 0 )
			{
				lua_pushnumber( L, gv ? memory::safe_read<float>( gv + 0x30 ).value_or( 0.0f ) : 0.0f );
				return 1;
			}
			if ( std::strcmp( key, "tickcount" ) == 0 )
			{
				lua_pushinteger( L, gv ? memory::safe_read<int>( gv + 0x44 ).value_or( 0 ) : 0 );
				return 1;
			}
			if ( std::strcmp( key, "tickinterval" ) == 0 )
			{
				lua_pushnumber( L, gv ? memory::safe_read<float>( gv + 0x34 ).value_or( cstypes::tick_interval ) : cstypes::tick_interval );
				return 1;
			}
			if ( std::strcmp( key, "realtime" ) == 0 )
			{
				lua_pushnumber( L, gv ? memory::safe_read<float>( gv + 0x00 ).value_or( 0.0f ) : 0.0f );
				return 1;
			}
			if ( std::strcmp( key, "framecount" ) == 0 )
			{
				lua_pushinteger( L, gv ? memory::safe_read<int>( gv + 0x04 ).value_or( 0 ) : 0 );
				return 1;
			}
			if ( std::strcmp( key, "frametime" ) == 0 )
			{
				lua_pushnumber( L, xdraw::delta_time( ) );
				return 1;
			}
			if ( std::strcmp( key, "max_players" ) == 0 )
			{
				const auto players = systems::g_entities.get_by_type( systems::entities::type::player );
				lua_pushinteger( L, static_cast< int >( std::max<std::size_t>( 1, players.size( ) ) ) );
				return 1;
			}
			if ( std::strcmp( key, "is_connected" ) == 0 )
			{
				lua_pushboolean( L, systems::g_local.get( ).controller != 0 );
				return 1;
			}
			if ( std::strcmp( key, "is_in_game" ) == 0 )
			{
				lua_pushboolean( L, systems::g_local.get( ).is_valid( ) );
				return 1;
			}

			lua_pushnil( L );
			return 1;
		}


		int l_render_screen_size( lua_State* L )
		{
			const auto [ w, h ] = xdraw::viewport_size( );
			push_vector( L, static_cast< float >( w ), static_cast< float >( h ) );
			return 1;
		}

		int l_render_camera_position( lua_State* L )
		{
			const auto origin = systems::g_view.origin( );
			push_vector( L, origin.x, origin.y, origin.z );
			return 1;
		}

		int l_render_camera_angles( lua_State* L )
		{
			const auto angles = systems::g_view.angles( );
			push_vector( L, angles.x, angles.y, angles.z );
			return 1;
		}

		int l_render_world_to_screen( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			if ( !systems::g_view.has_camera( ) )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto screen = systems::g_view.project( math::vector3{ v->x, v->y, v->z } );
			if ( screen.x == 57005.0f || screen.y == 57005.0f )
			{
				lua_pushnil( L );
				return 1;
			}

			push_vector( L, screen.x, screen.y );
			return 1;
		}

		int l_render_get_offscreen( lua_State* L )
		{
			const auto* v = check_vector( L, 1 );
			const auto radius_pct = static_cast< float >( luaL_optnumber( L, 2, 0.5f ) );
			const auto [ sw, sh ] = xdraw::viewport_size( );
			const auto cx = sw * 0.5f;
			const auto cy = sh * 0.5f;

			auto screen = systems::g_view.project( math::vector3{ v->x, v->y, v->z } );
			if ( screen.x == 57005.0f || screen.y == 57005.0f )
			{
				screen.x = cx;
				screen.y = cy;
			}

			const auto is_out =
				screen.x < 0.0f || screen.y < 0.0f ||
				screen.x > static_cast< float >( sw ) || screen.y > static_cast< float >( sh );

			if ( !is_out )
			{
				push_vector( L, screen.x, screen.y );
				lua_pushnumber( L, 0.0f );
				lua_pushboolean( L, false );
				return 3;
			}

			const auto ang = std::atan2f( screen.y - cy, screen.x - cx );
			const auto radius = static_cast< float >( std::min( sw, sh ) ) * 0.5f * std::clamp( radius_pct, 0.0f, 2.0f );
			push_vector( L, cx + std::cosf( ang ) * radius, cy + std::sinf( ang ) * radius );
			lua_pushnumber( L, ang * 180.0f / static_cast< float >( 3.14159265358979323846 ) );
			lua_pushboolean( L, true );
			return 3;
		}

		int l_render_measure_text( lua_State* L )
		{
			const auto text = arg_to_string( L, 2 );
			const auto [ w, h ] = xdraw::measure_text( text );
			push_vector( L, w, h );
			return 1;
		}

		int l_render_rect( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float ax, ay, bx, by;
			read_pos( L, 1, ax, ay, consumed );
			read_pos( L, 1 + consumed, bx, by, consumed );
			auto col = read_color( L, 1 + consumed, consumed );

			const auto x = std::min( ax, bx );
			const auto y = std::min( ay, by );
			const auto w = std::fabsf( bx - ax );
			const auto h = std::fabsf( by - ay );

			if ( w <= 0.0f || h <= 0.0f )
			{
				return 0;
			}

			const auto rounding = static_cast< float >( luaL_optnumber( L, 1 + consumed, 0.0f ) );
			if ( rounding > 0.0f )
			{
				current_paint( ).draw_list->rect_filled( x, y, w, h, col, xdraw::corner_radius{ rounding } );
			}
			else
			{
				current_paint( ).draw_list->rect_filled( x, y, w, h, col );
			}
			return 0;
		}

		int l_render_rect_outline( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float ax, ay, bx, by;
			read_pos( L, 1, ax, ay, consumed );
			read_pos( L, 1 + consumed, bx, by, consumed );
			auto col = read_color( L, 1 + consumed, consumed );

			const auto x = std::min( ax, bx );
			const auto y = std::min( ay, by );
			const auto w = std::fabsf( bx - ax );
			const auto h = std::fabsf( by - ay );

			const auto thickness = static_cast< float >( luaL_optnumber( L, 1 + consumed, 1.0f ) );
			const auto rounding = static_cast< float >( luaL_optnumber( L, 2 + consumed, 0.0f ) );

			if ( rounding > 0.0f )
			{
				current_paint( ).draw_list->rect( x, y, w, h, col, xdraw::corner_radius{ rounding }, thickness );
			}
			else
			{
				current_paint( ).draw_list->rect( x, y, w, h, col, thickness );
			}
			return 0;
		}

		int l_render_gradient( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float ax, ay, bx, by;
			read_pos( L, 1, ax, ay, consumed );
			read_pos( L, 1 + consumed, bx, by, consumed );

			const auto tl = read_color( L, 1 + consumed, consumed );
			const auto tr = read_color( L, 1 + consumed, consumed );
			const auto bl = read_color( L, 1 + consumed, consumed );
			const auto br = read_color( L, 1 + consumed, consumed );

			current_paint( ).draw_list->rect_filled_gradient(
				std::min( ax, bx ), std::min( ay, by ),
				std::fabsf( bx - ax ), std::fabsf( by - ay ),
				tl, tr, br, bl );
			return 0;
		}

		int l_render_line( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float ax, ay, bx, by;
			read_pos( L, 1, ax, ay, consumed );
			read_pos( L, 1 + consumed, bx, by, consumed );
			auto col = read_color( L, 1 + consumed, consumed );
			const auto thickness = static_cast< float >( luaL_optnumber( L, 1 + consumed, 1.0f ) );

			current_paint( ).draw_list->line( ax, ay, bx, by, col, thickness );
			return 0;
		}

		namespace {
			[[nodiscard]] std::vector<float> make_arc_points( float cx, float cy, float radius, float start_deg, float pct, int segments = 48 )
			{
				std::vector<float> points{};
				points.reserve( static_cast< std::size_t >( segments ) * 2 + 2 );

				const auto degrees = start_deg;
				for ( auto i = 0; i <= segments; ++i )
				{
					const auto t = degrees + 360.0f * pct * ( static_cast< float >( i ) / static_cast< float >( segments ) );
					const auto rad = t * static_cast< float >( 3.14159265358979323846 ) / 180.0f;
					points.push_back( cx + std::cosf( rad ) * radius );
					points.push_back( cy + std::sinf( rad ) * radius );
				}

				return points;
			}
		}

		int l_render_circle( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float cx, cy;
			read_pos( L, 1, cx, cy, consumed );
			auto col = read_color( L, 1 + consumed, consumed );
			const auto radius = static_cast< float >( luaL_checknumber( L, 1 + consumed ) );
			const auto start_deg = static_cast< float >( luaL_optnumber( L, 2 + consumed, 0.0f ) );
			const auto pct = static_cast< float >( luaL_optnumber( L, 3 + consumed, 1.0f ) );

			if ( pct >= 0.999f )
			{
				current_paint( ).draw_list->circle_filled( cx, cy, radius, col );
				return 0;
			}

			auto points = make_arc_points( cx, cy, radius, start_deg, std::clamp( pct, 0.0f, 1.0f ) );
			std::vector<float> fan{};
			fan.reserve( points.size( ) + 2 );
			fan.push_back( cx );
			fan.push_back( cy );
			fan.insert( fan.end( ), points.begin( ), points.end( ) );
			current_paint( ).draw_list->convex_filled( fan, col );
			return 0;
		}

		int l_render_circle_outline( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float cx, cy;
			read_pos( L, 1, cx, cy, consumed );
			auto col = read_color( L, 1 + consumed, consumed );
			const auto radius = static_cast< float >( luaL_checknumber( L, 1 + consumed ) );
			const auto start_deg = static_cast< float >( luaL_optnumber( L, 2 + consumed, 0.0f ) );
			const auto pct = static_cast< float >( luaL_optnumber( L, 3 + consumed, 1.0f ) );
			const auto thickness = static_cast< float >( luaL_optnumber( L, 4 + consumed, 1.0f ) );

			if ( pct >= 0.999f )
			{
				current_paint( ).draw_list->circle( cx, cy, radius, col, thickness );
				return 0;
			}

			auto points = make_arc_points( cx, cy, radius, start_deg, std::clamp( pct, 0.0f, 1.0f ) );
			current_paint( ).draw_list->polyline( points, col, false, thickness );
			return 0;
		}

		int l_render_circle_3d( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto* v = check_vector( L, 1 );
			const auto screen = systems::g_view.project( math::vector3{ v->x, v->y, v->z } );
			if ( screen.x == 57005.0f || screen.y == 57005.0f )
			{
				return 0;
			}

			int consumed{};
			auto col = read_color( L, 2, consumed );
			const auto radius = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto outline = lua_toboolean( L, 6 );

			if ( outline )
			{
				current_paint( ).draw_list->circle( screen.x, screen.y, radius, col, 1.0f );
			}
			else
			{
				current_paint( ).draw_list->circle_filled( screen.x, screen.y, radius, col );
			}
			return 0;
		}

		int l_render_circle_3d_outline( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto* v = check_vector( L, 1 );
			const auto screen = systems::g_view.project( math::vector3{ v->x, v->y, v->z } );
			if ( screen.x == 57005.0f || screen.y == 57005.0f )
			{
				return 0;
			}

			int consumed{};
			auto col = read_color( L, 2, consumed );
			const auto radius = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto thickness = static_cast< float >( luaL_optnumber( L, 6, 1.0f ) );

			current_paint( ).draw_list->circle( screen.x, screen.y, radius, col, thickness );
			return 0;
		}

		int l_render_text( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int arg = 2;

			int consumed{};
			float x, y;
			read_pos( L, arg, x, y, consumed );
			arg += consumed;
			auto col = read_color( L, arg, consumed );
			arg += consumed;

			std::string flags{};
			if ( !lua_isnil( L, arg ) && lua_type( L, arg ) == LUA_TSTRING )
			{
				flags = lua_tostring( L, arg );
			}
			++arg;

			std::string out{};
			for ( auto i = arg; i <= lua_gettop( L ); ++i )
			{
				if ( !out.empty( ) )
				{
					out += " ";
				}
				out += arg_to_string( L, i );
			}

			if ( flags.find( 'c' ) != std::string::npos || flags.find( 'r' ) != std::string::npos )
			{
				const auto [ w, h ] = xdraw::measure_text( out );
				if ( flags.find( 'c' ) != std::string::npos )
				{
					x -= w * 0.5f;
				}
				if ( flags.find( 'r' ) != std::string::npos )
				{
					x -= w;
				}
			}

			current_paint( ).draw_list->text( x, y, out, col );
			return 0;
		}

		int l_render_push_clip( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			int consumed{};
			float ax, ay, bx, by;
			read_pos( L, 1, ax, ay, consumed );
			read_pos( L, 1 + consumed, bx, by, consumed );

			current_paint( ).draw_list->push_clip(
				std::min( ax, bx ), std::min( ay, by ),
				std::fabsf( bx - ax ), std::fabsf( by - ay ) );
			return 0;
		}

		int l_render_pop_clip( lua_State* L )
		{
			if ( current_paint( ).draw_list )
			{
				current_paint( ).draw_list->pop_clip( );
			}
			return 0;
		}


		int l_cvar_index( lua_State* L )
		{
			if ( lua_type( L, 2 ) != LUA_TSTRING )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto* name = lua_tostring( L, 2 );
			if ( !addresses::globals::cvar )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto hash = fnv1a::runtime_hash( name );
			const auto cvar = addresses::globals::cvar->find( hash );
			if ( !cvar )
			{
				lua_pushnil( L );
				return 1;
			}

			push_cvar( L, reinterpret_cast< std::uintptr_t >( cvar ), name );
			return 1;
		}

		int l_cvar_methods_index( lua_State* L )
		{
			lua_pushvalue( L, lua_upvalueindex( 1 ) );
			lua_pushvalue( L, 2 );
			lua_gettable( L, -2 );
			return 1;
		}

		int l_cvar_int( lua_State* L )
		{
			auto* c = check_cvar( L, 1 );
			auto* cv = reinterpret_cast< c_convar* >( c->ptr );
			if ( !cv )
			{
				lua_pushinteger( L, 0 );
				return 1;
			}

			if ( lua_gettop( L ) >= 2 && !lua_isnil( L, 2 ) )
			{
				cv->m_value.i32 = static_cast< int >( luaL_checkinteger( L, 2 ) );
				return 0;
			}

			lua_pushinteger( L, static_cast< int >( cv->m_value.i32 ) );
			return 1;
		}

		int l_cvar_float( lua_State* L )
		{
			auto* c = check_cvar( L, 1 );
			auto* cv = reinterpret_cast< c_convar* >( c->ptr );
			if ( !cv )
			{
				lua_pushnumber( L, 0.0f );
				return 1;
			}

			if ( lua_gettop( L ) >= 2 && !lua_isnil( L, 2 ) )
			{
				cv->m_value.fl = static_cast< float >( luaL_checknumber( L, 2 ) );
				return 0;
			}

			lua_pushnumber( L, cv->m_value.fl );
			return 1;
		}

		int l_cvar_bool( lua_State* L )
		{
			auto* c = check_cvar( L, 1 );
			auto* cv = reinterpret_cast< c_convar* >( c->ptr );
			if ( !cv )
			{
				lua_pushboolean( L, false );
				return 1;
			}

			if ( lua_gettop( L ) >= 2 && !lua_isnil( L, 2 ) )
			{
				cv->m_value.i1 = lua_toboolean( L, 2 ) != 0;
				return 0;
			}

			lua_pushboolean( L, cv->m_value.i1 != 0 );
			return 1;
		}

		int l_cvar_string( lua_State* L )
		{
			auto* c = check_cvar( L, 1 );
			auto* cv = reinterpret_cast< c_convar* >( c->ptr );
			if ( !cv )
			{
				lua_pushstring( L, "" );
				return 1;
			}

			if ( lua_gettop( L ) >= 2 && !lua_isnil( L, 2 ) )
			{
				static std::unordered_map<std::uintptr_t, std::string> storage{};
				const auto value = arg_to_string( L, 2 );
				storage[ c->ptr ] = value;
				cv->m_value.sz = storage[ c->ptr ].c_str( );
				return 0;
			}

			const auto ptr = memory::safe_read<const char*>( reinterpret_cast< std::uintptr_t >( cv ) + offsetof( c_convar, m_value ) );
			const auto value = ptr ? memory::read_string( reinterpret_cast< std::uintptr_t >( *ptr ), 511 ) : std::string{};
			lua_pushstring( L, value.c_str( ) );
			return 1;
		}

		int l_cvar_call( lua_State* L )
		{
			auto* c = check_cvar( L, 1 );
			if ( !c || !c->name[ 0 ] )
			{
				return 0;
			}

			std::string command = c->name;
			for ( auto i = 2; i <= lua_gettop( L ); ++i )
			{
				const auto arg = arg_to_string( L, i );
				if ( !arg.empty( ) )
				{
					command += " ";
					command += arg;
				}
			}

			addresses::globals::execute_client_cmd( command.c_str( ), 0x7ffef001 );
			return 0;
		}


		int l_entity_index( lua_State* L )
		{
			check_entity( L, 1 );
			if ( lua_type( L, 2 ) != LUA_TSTRING )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto* key = lua_tostring( L, 2 );

			lua_pushvalue( L, lua_upvalueindex( 1 ) );
			lua_pushvalue( L, 2 );
			lua_gettable( L, -2 );
			if ( lua_isfunction( L, -1 ) )
			{
				return 1;
			}
			lua_pop( L, 1 );

			auto* e = static_cast< ent_data* >( luaL_checkudata( L, 1, k_entity_mt ) );
			const auto prop = resolve_entity_prop( *e, key );
			if ( !prop.found || !prop.base )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto addr = prop.base + static_cast< std::uintptr_t >( prop.offset );
			switch ( prop_kind_from_name( key ) )
			{
			case prop_kind::boolean:
				lua_pushboolean( L, memory::safe_read<bool>( addr ).value_or( false ) );
				return 1;
			case prop_kind::floating:
				lua_pushnumber( L, memory::safe_read<float>( addr ).value_or( 0.0f ) );
				return 1;
			case prop_kind::vector:
			{
				const auto vec = memory::safe_read<math::vector3>( addr ).value_or( math::vector3{} );
				push_vector( L, vec.x, vec.y, vec.z );
				return 1;
			}
			case prop_kind::string:
			{
				const auto str_ptr = memory::safe_read<const char*>( addr ).value_or( nullptr );
				if ( str_ptr && reinterpret_cast< std::uintptr_t >( str_ptr ) > 0x10000ull )
				{
					lua_pushstring( L, memory::read_string( reinterpret_cast< std::uintptr_t >( str_ptr ), 511 ).c_str( ) );
				}
				else
				{
					lua_pushstring( L, "" );
				}
				return 1;
			}
			case prop_kind::int64:
				lua_pushnumber( L, static_cast< double >( memory::safe_read<std::int64_t>( addr ).value_or( 0 ) ) );
				return 1;
			case prop_kind::integer:
			default:
				lua_pushinteger( L, memory::safe_read<int>( addr ).value_or( 0 ) );
				return 1;
			}
		}

		int l_entity_newindex( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			if ( lua_type( L, 2 ) != LUA_TSTRING )
			{
				return 0;
			}

			const auto* key = lua_tostring( L, 2 );
			const auto prop = resolve_entity_prop( *e, key );
			if ( !prop.found || !prop.base )
			{
				return 0;
			}

			const auto addr = prop.base + static_cast< std::uintptr_t >( prop.offset );
			switch ( prop_kind_from_name( key ) )
			{
			case prop_kind::boolean:
				static_cast<void>( memory::safe_write<bool>( addr, lua_toboolean( L, 3 ) != 0 ) );
				return 0;
			case prop_kind::floating:
				static_cast<void>( memory::safe_write<float>( addr, static_cast< float >( luaL_checknumber( L, 3 ) ) ) );
				return 0;
			case prop_kind::vector:
			{
				if ( lua_type( L, 3 ) == LUA_TUSERDATA )
				{
					const auto* v = check_vector( L, 3 );
					static_cast<void>( memory::safe_write<math::vector3>( addr, math::vector3{ v->x, v->y, v->z } ) );
				}
				else
				{
					static_cast<void>( memory::safe_write<math::vector3>( addr, math::vector3{
						static_cast< float >( luaL_checknumber( L, 3 ) ),
						static_cast< float >( luaL_checknumber( L, 4 ) ),
						static_cast< float >( luaL_checknumber( L, 5 ) ) } ) );
				}
				return 0;
			}
			case prop_kind::integer:
			default:
				static_cast<void>( memory::safe_write<int>( addr, static_cast< int >( luaL_checkinteger( L, 3 ) ) ) );
				return 0;
			}
		}

		int l_entity_is_player( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			lua_pushboolean( L, e->pawn != 0 || e->controller != 0 );
			return 1;
		}

		int l_entity_is_enemy( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			const auto local = systems::g_local.get( );
			if ( !local.is_valid( ) || !e->pawn )
			{
				lua_pushboolean( L, false );
				return 1;
			}
			const auto team = memory::safe_read<int>( e->pawn + prop_offset( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
			lua_pushboolean( L, local.is_this_other_team( team ) );
			return 1;
		}

		int l_entity_is_alive( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			auto alive = false;
			if ( e->pawn )
			{
				alive = memory::safe_read<int>( e->pawn + prop_offset( "C_BaseEntity", "m_iHealth"_hash ) ).value_or( 0 ) > 0;
			}
			if ( !alive && e->controller )
			{
				alive = memory::safe_read<bool>( e->controller + prop_offset( "CCSPlayerController", "m_bPawnIsAlive"_hash ) ).value_or( false );
			}
			lua_pushboolean( L, alive );
			return 1;
		}

		int l_entity_get_index( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			lua_pushinteger( L, e->index );
			return 1;
		}

		int l_entity_get_health( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			lua_pushinteger( L, e->pawn ? memory::safe_read<int>( e->pawn + prop_offset( "C_BaseEntity", "m_iHealth"_hash ) ).value_or( 0 ) : 0 );
			return 1;
		}

		int l_entity_get_team( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			lua_pushinteger( L, e->pawn ? memory::safe_read<int>( e->pawn + prop_offset( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 ) : 0 );
			return 1;
		}

		int l_entity_get_flags( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			lua_pushinteger( L, e->pawn ? memory::safe_read<int>( e->pawn + prop_offset( "C_CSPlayerPawnBase", "m_fFlags"_hash ) ).value_or( 0 ) : 0 );
			return 1;
		}

		int l_entity_get_velocity( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			if ( !e->pawn )
			{
				push_vector( L, 0.0f, 0.0f, 0.0f );
				return 1;
			}
			const auto vel = memory::safe_read<math::vector3>( e->pawn + prop_offset( "C_BaseEntity", "m_vecAbsVelocity"_hash ) ).value_or( math::vector3{} );
			push_vector( L, vel.x, vel.y, vel.z );
			return 1;
		}

		int l_entity_get_speed( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			if ( !e->pawn )
			{
				lua_pushnumber( L, 0.0f );
				return 1;
			}
			const auto vel = memory::safe_read<math::vector3>( e->pawn + prop_offset( "C_BaseEntity", "m_vecAbsVelocity"_hash ) ).value_or( math::vector3{} );
			lua_pushnumber( L, vel.length( ) );
			return 1;
		}

		int l_entity_get_origin( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			if ( !e->pawn )
			{
				push_vector( L, 0.0f, 0.0f, 0.0f );
				return 1;
			}
			const auto node = memory::safe_read<std::uintptr_t>( e->pawn + prop_offset( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
			if ( !node )
			{
				push_vector( L, 0.0f, 0.0f, 0.0f );
				return 1;
			}
			const auto origin = memory::safe_read<math::vector3>( node + prop_offset( "CGameSceneNode", "m_vecAbsOrigin"_hash ) ).value_or( math::vector3{} );
			push_vector( L, origin.x, origin.y, origin.z );
			return 1;
		}

		int l_entity_get_name( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			if ( e->controller )
			{
				const auto name_ptr = memory::safe_read<std::uintptr_t>( e->controller + prop_offset( "CCSPlayerController", "m_sSanitizedPlayerName"_hash ) ).value_or( 0 );
				if ( name_ptr )
				{
					lua_pushstring( L, memory::read_string( name_ptr, 127 ).c_str( ) );
					return 1;
				}
			}
			lua_pushstring( L, "" );
			return 1;
		}

		int l_entity_get_ping( lua_State* L )
		{
			auto* e = check_entity( L, 1 );
			lua_pushinteger( L, e->controller ? memory::safe_read<int>( e->controller + prop_offset( "CCSPlayerController", "m_iPing"_hash ) ).value_or( 0 ) : 0 );
			return 1;
		}

		int l_entity_get_local_player( lua_State* L )
		{
			const auto local = systems::g_local.get( );
			if ( !local.pawn && !local.controller )
			{
				lua_pushnil( L );
				return 1;
			}
			push_entity( L, local.pawn, local.controller, 0 );
			return 1;
		}

		int l_entity_get_players( lua_State* L )
		{
			const auto enemies_only = lua_toboolean( L, 1 );
			const auto local = systems::g_local.get( );

			lua_createtable( L, 0, 16 );
			auto count = 1;

			for ( const auto& cached : systems::g_entities.get_by_type( systems::entities::type::player ) )
			{
				const auto controller = cached.ptr;
				if ( !controller )
				{
					continue;
				}

				const auto pawn_handle = memory::safe_read<std::uint32_t>( controller + prop_offset( "CBasePlayerController", "m_hPawn"_hash ) ).value_or( 0 );
				const auto pawn = pawn_handle != 0 && pawn_handle != 0xffffffffu
					? systems::g_entities.lookup( pawn_handle )
					: std::uintptr_t{};

				if ( enemies_only && pawn )
				{
					const auto team = memory::safe_read<int>( pawn + prop_offset( "C_BaseEntity", "m_iTeamNum"_hash ) ).value_or( 0 );
					if ( !local.is_this_other_team( team ) )
					{
						continue;
					}
				}

				push_entity( L, pawn, controller, cached.index );
				lua_rawseti( L, -2, count++ );
			}

			return 1;
		}

		int l_entity_get( lua_State* L )
		{
			const auto idx = static_cast< int >( luaL_checkinteger( L, 1 ) );
			const auto by_userid = lua_toboolean( L, 2 );

			std::uintptr_t ptr{};
			if ( by_userid )
			{
				for ( const auto& cached : systems::g_entities.get_by_type( systems::entities::type::player ) )
				{
					if ( cached.index == idx )
					{
						ptr = cached.ptr;
						break;
					}
				}
			}
			else
			{
				ptr = systems::g_entities.get_by_index( idx );
			}

			if ( !ptr )
			{
				lua_pushnil( L );
				return 1;
			}

			const auto* schema_name = systems::g_entities.get_schema_name( ptr );
			const auto is_controller = schema_name && std::strstr( schema_name, "Controller" ) != nullptr;

			if ( is_controller )
			{
				const auto pawn_handle = memory::safe_read<std::uint32_t>( ptr + prop_offset( "CBasePlayerController", "m_hPawn"_hash ) ).value_or( 0 );
				const auto pawn = pawn_handle != 0 && pawn_handle != 0xffffffffu
					? systems::g_entities.lookup( pawn_handle )
					: std::uintptr_t{};
				push_entity( L, pawn, ptr, static_cast< std::int16_t >( idx ) );
				return 1;
			}

			const auto is_pawn = schema_name && std::strstr( schema_name, "Pawn" ) != nullptr;
			if ( is_pawn )
			{
				const auto ctrl_handle = memory::safe_read<std::uint32_t>( ptr + prop_offset( "C_BasePlayerPawn", "m_hController"_hash ) ).value_or( 0 );
				const auto controller = ctrl_handle != 0 && ctrl_handle != 0xffffffffu
					? systems::g_entities.lookup( ctrl_handle )
					: std::uintptr_t{};
				push_entity( L, ptr, controller, static_cast< std::int16_t >( idx ) );
				return 1;
			}

			push_entity( L, ptr, 0, static_cast< std::int16_t >( idx ) );
			return 1;
		}


		int l_events_index( lua_State* L )
		{
			if ( lua_type( L, 2 ) != LUA_TSTRING )
			{
				lua_pushnil( L );
				return 1;
			}
			push_event( L, lua_tostring( L, 2 ) );
			return 1;
		}

		int l_event_set( lua_State* L )
		{
			auto* e = check_event( L, 1 );
			luaL_checktype( L, 2, LUA_TFUNCTION );
			const auto ref = luaL_ref( L, LUA_REGISTRYINDEX );
			g_event_refs[ e->name ].push_back( ref );
			return 0;
		}

		int l_event_unset( lua_State* L )
		{
			auto* e = check_event( L, 1 );
			if ( lua_type( L, 2 ) != LUA_TFUNCTION || !lua_isnil( L, 2 ) )
			{
				const auto it = g_event_refs.find( e->name );
				if ( it != g_event_refs.end( ) )
				{
					for ( auto rit = it->second.begin( ); rit != it->second.end( ); )
					{
						lua_rawgeti( L, LUA_REGISTRYINDEX, *rit );
						const auto equal = lua_rawequal( L, -1, 2 ) != 0;
						lua_pop( L, 1 );
						if ( equal )
						{
							luaL_unref( L, LUA_REGISTRYINDEX, *rit );
							rit = it->second.erase( rit );
						}
						else
						{
							++rit;
						}
					}
				}
			}
			return 0;
		}

		void fire_event( lua_State* L, const char* name )
		{
			if ( !L || !name )
			{
				return;
			}

			const auto it = g_event_refs.find( name );
			if ( it == g_event_refs.end( ) )
			{
				return;
			}

			for ( const auto ref : it->second )
			{
				lua_rawgeti( L, LUA_REGISTRYINDEX, ref );
				if ( !lua_isfunction( L, -1 ) )
				{
					lua_pop( L, 1 );
					continue;
				}
				if ( lua_pcall( L, 0, 0, 0 ) != LUA_OK )
				{
					lua_pop( L, 1 );
				}
			}
		}


		void output_log( lua_State* L )
		{
			std::string out{};
			const auto count = static_cast< int >( lua_gettop( L ) );
			for ( auto i = 1; i <= count; ++i )
			{
				if ( !out.empty( ) )
				{
					out += " ";
				}
				switch ( lua_type( L, i ) )
				{
				case LUA_TNUMBER:
					out += std::to_string( lua_tonumber( L, i ) );
					break;
				case LUA_TBOOLEAN:
					out += lua_toboolean( L, i ) ? "true" : "false";
					break;
				case LUA_TSTRING:
					out += lua_tostring( L, i );
					break;
				case LUA_TNIL:
					out += "nil";
					break;
				default:
					out += "<" + std::string( lua_typename( L, lua_type( L, i ) ) ) + ">";
					break;
				}
			}

			diag::writef( diag::level::info, "[lua] script: %s", out.c_str( ) );
			OutputDebugStringA( ( "aetheris lua: " + out + "\n" ).c_str( ) );
		}

		int l_client_log( lua_State* L )
		{
			output_log( L );
			return 0;
		}

		int l_client_get_latency( lua_State* L )
		{
			auto ping{ 0 };
			const auto local = systems::g_local.get( );
			if ( local.controller )
			{
				ping = memory::safe_read<int>( local.controller + prop_offset( "CCSPlayerController", "m_iPing"_hash ) ).value_or( 0 );
			}
			lua_pushinteger( L, ping );
			return 1;
		}

		int l_client_get_name( lua_State* L )
		{
			const auto* name = steam::friends::get_persona_name( );
			lua_pushstring( L, name ? name : "unknown" );
			return 1;
		}


		int l_push_number( lua_State* L )
		{
			const auto count = static_cast< int >( lua_gettop( L ) );
			for ( auto i = 1; i <= count; ++i )
			{
				lua_pushnumber( L, lua_tonumber( L, i ) );
			}
			return count;
		}

		int l_delta_time( lua_State* L )
		{
			lua_pushnumber( L, xdraw::delta_time( ) );
			return 1;
		}

		int l_framerate( lua_State* L )
		{
			lua_pushnumber( L, xdraw::framerate( ) );
			return 1;
		}

		int l_viewport( lua_State* L )
		{
			const auto [ w, h ] = xdraw::viewport_size( );
			lua_pushinteger( L, w );
			lua_pushinteger( L, h );
			return 2;
		}

		int l_color_legacy( lua_State* L )
		{
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 1 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 2 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 3 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 4 ? static_cast< std::uint8_t >( luaL_checknumber( L, 4 ) ) : 255 );

			lua_createtable( L, 0, 4 );
			lua_pushinteger( L, r );
			lua_setfield( L, -2, "r" );
			lua_pushinteger( L, g );
			lua_setfield( L, -2, "g" );
			lua_pushinteger( L, b );
			lua_setfield( L, -2, "b" );
			lua_pushinteger( L, a );
			lua_setfield( L, -2, "a" );
			return 1;
		}

		int l_velocity( lua_State* L )
		{
			if ( !current_paint( ).pawn )
			{
				lua_pushnumber( L, 0.0f );
				lua_pushnumber( L, 0.0f );
				lua_pushnumber( L, 0.0f );
				return 3;
			}

			const auto velocity = memory::read<math::vector3>( current_paint( ).pawn + SCHEMA( "C_BaseEntity", "m_vecAbsVelocity"_hash ) );
			lua_pushnumber( L, velocity.x );
			lua_pushnumber( L, velocity.y );
			lua_pushnumber( L, velocity.z );
			return 3;
		}

		int l_speed( lua_State* L )
		{
			if ( !current_paint( ).pawn )
			{
				lua_pushnumber( L, 0.0f );
				return 1;
			}

			const auto velocity = memory::read<math::vector3>( current_paint( ).pawn + SCHEMA( "C_BaseEntity", "m_vecAbsVelocity"_hash ) );
			lua_pushnumber( L, velocity.length( ) );
			return 1;
		}

		int l_health( lua_State* L )
		{
			if ( !current_paint( ).pawn )
			{
				lua_pushinteger( L, 0 );
				return 1;
			}

			lua_pushinteger( L, memory::read<int>( current_paint( ).pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) ) );
			return 1;
		}

		int l_flags( lua_State* L )
		{
			if ( !current_paint( ).pawn )
			{
				lua_pushinteger( L, 0 );
				return 1;
			}

			lua_pushinteger( L, memory::read<int>( current_paint( ).pawn + SCHEMA( "C_CSPlayerPawnBase", "m_fFlags"_hash ) ) );
			return 1;
		}

		int l_alive( lua_State* L )
		{
			const auto local = systems::g_local.get( );
			lua_pushboolean( L, current_paint( ).pawn != 0 && local.is_alive );
			return 1;
		}

		int l_rect_filled( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto x = static_cast< float >( luaL_checknumber( L, 1 ) );
			const auto y = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto w = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto h = static_cast< float >( luaL_checknumber( L, 4 ) );
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 5 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 6 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 7 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 8 ? static_cast< std::uint8_t >( luaL_checknumber( L, 8 ) ) : 255 );

			current_paint( ).draw_list->rect_filled( x, y, w, h, xdraw::color{ r, g, b, a } );
			return 0;
		}

		int l_rect( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto x = static_cast< float >( luaL_checknumber( L, 1 ) );
			const auto y = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto w = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto h = static_cast< float >( luaL_checknumber( L, 4 ) );
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 5 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 6 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 7 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 8 ? static_cast< std::uint8_t >( luaL_checknumber( L, 8 ) ) : 255 );
			const auto thickness = lua_gettop( L ) >= 9 ? static_cast< float >( luaL_checknumber( L, 9 ) ) : 1.0f;

			current_paint( ).draw_list->rect( x, y, w, h, xdraw::color{ r, g, b, a }, thickness );
			return 0;
		}

		int l_line( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto x0 = static_cast< float >( luaL_checknumber( L, 1 ) );
			const auto y0 = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto x1 = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto y1 = static_cast< float >( luaL_checknumber( L, 4 ) );
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 5 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 6 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 7 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 8 ? static_cast< std::uint8_t >( luaL_checknumber( L, 8 ) ) : 255 );
			const auto thickness = lua_gettop( L ) >= 9 ? static_cast< float >( luaL_checknumber( L, 9 ) ) : 1.0f;

			current_paint( ).draw_list->line( x0, y0, x1, y1, xdraw::color{ r, g, b, a }, thickness );
			return 0;
		}

		int l_circle( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto cx = static_cast< float >( luaL_checknumber( L, 1 ) );
			const auto cy = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto radius = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 4 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 5 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 6 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 7 ? static_cast< std::uint8_t >( luaL_checknumber( L, 7 ) ) : 255 );
			const auto thickness = lua_gettop( L ) >= 8 ? static_cast< float >( luaL_checknumber( L, 8 ) ) : 1.0f;

			current_paint( ).draw_list->circle( cx, cy, radius, xdraw::color{ r, g, b, a }, thickness );
			return 0;
		}

		int l_circle_filled( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto cx = static_cast< float >( luaL_checknumber( L, 1 ) );
			const auto cy = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto radius = static_cast< float >( luaL_checknumber( L, 3 ) );
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 4 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 5 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 6 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 7 ? static_cast< std::uint8_t >( luaL_checknumber( L, 7 ) ) : 255 );

			current_paint( ).draw_list->circle_filled( cx, cy, radius, xdraw::color{ r, g, b, a } );
			return 0;
		}

		int l_text( lua_State* L )
		{
			if ( !current_paint( ).draw_list )
			{
				return 0;
			}

			const auto x = static_cast< float >( luaL_checknumber( L, 1 ) );
			const auto y = static_cast< float >( luaL_checknumber( L, 2 ) );
			const auto* str = luaL_checkstring( L, 3 );
			const auto r = static_cast< std::uint8_t >( luaL_checknumber( L, 4 ) );
			const auto g = static_cast< std::uint8_t >( luaL_checknumber( L, 5 ) );
			const auto b = static_cast< std::uint8_t >( luaL_checknumber( L, 6 ) );
			const auto a = static_cast< std::uint8_t >( lua_gettop( L ) >= 7 ? static_cast< std::uint8_t >( luaL_checknumber( L, 7 ) ) : 255 );

			current_paint( ).draw_list->text( x, y, str, xdraw::color{ r, g, b, a } );
			return 0;
		}

		int l_text_size( lua_State* L )
		{
			const auto* str = luaL_checkstring( L, 1 );
			const auto [ w, h ] = xdraw::measure_text( str );
			lua_pushnumber( L, w );
			lua_pushnumber( L, h );
			return 2;
		}

		int l_log( lua_State* L )
		{
			output_log( L );
			return 0;
		}

		int l_config_name( lua_State* L )
		{
			const auto& name = g_lua.config_name( );
			lua_pushstring( L, name.empty( ) ? "default" : name.c_str( ) );
			return 1;
		}

		int l_ping( lua_State* L )
		{
			auto ping{ 0 };
			const auto local = systems::g_local.get( );
			if ( local.controller )
			{
				ping = memory::read<int>( local.controller + SCHEMA( "CCSPlayerController", "m_iPing"_hash ) );
			}
			lua_pushinteger( L, ping );
			return 1;
		}

		int l_steam_name( lua_State* L )
		{
			const auto* name = steam::friends::get_persona_name( );
			lua_pushstring( L, name ? name : "unknown" );
			return 1;
		}

		constexpr luaL_Reg k_velo_funcs[] = {
			{ "delta_time", l_delta_time },
			{ "framerate", l_framerate },
			{ "viewport", l_viewport },
			{ "color", l_color_legacy },
			{ "velocity", l_velocity },
			{ "speed", l_speed },
			{ "health", l_health },
			{ "flags", l_flags },
			{ "alive", l_alive },
			{ "rect_filled", l_rect_filled },
			{ "rect", l_rect },
			{ "line", l_line },
			{ "circle", l_circle },
			{ "circle_filled", l_circle_filled },
			{ "text", l_text },
			{ "text_size", l_text_size },
			{ "log", l_log },
			{ "config_name", l_config_name },
			{ "ping", l_ping },
			{ "steam_name", l_steam_name },
			{ nullptr, nullptr }
		};


		void push_method( lua_State* L, const char* name, lua_CFunction fn )
		{
			lua_pushcfunction( L, fn );
			lua_setfield( L, -2, name );
		}

		void register_types( lua_State* L )
		{
			luaL_newmetatable( L, k_vector_mt );
			{
				lua_createtable( L, 0, 8 );
				push_method( L, "length", l_vector_length );
				push_method( L, "length2d", l_vector_length2d );
				push_method( L, "length_sqr", l_vector_length_sqr );
				push_method( L, "copy", l_vector_copy );
				push_method( L, "set", l_vector_set );
				push_method( L, "to_screen", l_vector_to_screen );
				lua_pushcclosure( L, l_vector_index, 1 );
				lua_setfield( L, -2, "__index" );
				lua_pushcfunction( L, l_vector_newindex );
				lua_setfield( L, -2, "__newindex" );
				lua_pushcfunction( L, l_vector_add );
				lua_setfield( L, -2, "__add" );
				lua_pushcfunction( L, l_vector_sub );
				lua_setfield( L, -2, "__sub" );
				lua_pushcfunction( L, l_vector_mul );
				lua_setfield( L, -2, "__mul" );
				lua_pushcfunction( L, l_vector_div );
				lua_setfield( L, -2, "__div" );
				lua_pushcfunction( L, l_vector_unm );
				lua_setfield( L, -2, "__unm" );
				lua_pushcfunction( L, l_vector_eq );
				lua_setfield( L, -2, "__eq" );
				lua_pushcfunction( L, l_vector_tostring );
				lua_setfield( L, -2, "__tostring" );
			}
			lua_pop( L, 1 );

			luaL_newmetatable( L, k_color_mt );
			{
				lua_pushcfunction( L, l_color_index );
				lua_setfield( L, -2, "__index" );
				lua_pushcfunction( L, l_color_newindex );
				lua_setfield( L, -2, "__newindex" );
				lua_pushcfunction( L, l_color_tostring );
				lua_setfield( L, -2, "__tostring" );
				lua_pushcfunction( L, l_color_eq );
				lua_setfield( L, -2, "__eq" );
			}
			lua_pop( L, 1 );

			luaL_newmetatable( L, k_cvar_mt );
			{
				lua_createtable( L, 0, 6 );
				push_method( L, "int", l_cvar_int );
				push_method( L, "float", l_cvar_float );
				push_method( L, "bool", l_cvar_bool );
				push_method( L, "string", l_cvar_string );
				push_method( L, "call", l_cvar_call );
				lua_pushcclosure( L, l_cvar_methods_index, 1 );
				lua_setfield( L, -2, "__index" );
			}
			lua_pop( L, 1 );

			luaL_newmetatable( L, k_entity_mt );
			{
				lua_createtable( L, 0, 16 );
				push_method( L, "is_player", l_entity_is_player );
				push_method( L, "is_enemy", l_entity_is_enemy );
				push_method( L, "is_alive", l_entity_is_alive );
				push_method( L, "get_index", l_entity_get_index );
				push_method( L, "get_health", l_entity_get_health );
				push_method( L, "get_team", l_entity_get_team );
				push_method( L, "get_flags", l_entity_get_flags );
				push_method( L, "get_velocity", l_entity_get_velocity );
				push_method( L, "get_speed", l_entity_get_speed );
				push_method( L, "get_origin", l_entity_get_origin );
				push_method( L, "get_name", l_entity_get_name );
				push_method( L, "get_ping", l_entity_get_ping );
				lua_pushcclosure( L, l_entity_index, 1 );
				lua_setfield( L, -2, "__index" );
				lua_pushcfunction( L, l_entity_newindex );
				lua_setfield( L, -2, "__newindex" );
			}
			lua_pop( L, 1 );

			luaL_newmetatable( L, k_event_mt );
			{
				lua_pushcfunction( L, l_event_set );
				lua_setfield( L, -2, "set" );
				lua_pushcfunction( L, l_event_unset );
				lua_setfield( L, -2, "unset" );
			}
			lua_pop( L, 1 );

			lua_newtable( L );
			{
				lua_pushcfunction( L, l_vector_call );
				lua_setfield( L, -2, "__call" );
			}
			lua_setmetatable( L, -2 );
			lua_setglobal( L, "vector" );

			lua_newtable( L );
			{
				lua_pushcfunction( L, l_color_call );
				lua_setfield( L, -2, "__call" );
			}
			lua_setmetatable( L, -2 );

			constexpr struct { const char* name; std::uint8_t r, g, b; } k_colors[] = {
				{ "white", 255, 255, 255 },
				{ "black", 0, 0, 0 },
				{ "red", 255, 0, 0 },
				{ "green", 0, 255, 0 },
				{ "blue", 0, 0, 255 },
				{ "yellow", 255, 255, 0 },
				{ "orange", 255, 128, 0 },
				{ "purple", 128, 0, 255 },
				{ "pink", 255, 0, 255 },
				{ "grey", 128, 128, 128 },
				{ "gray", 128, 128, 128 },
				{ "silver", 192, 192, 192 },
				{ "gold", 255, 215, 0 },
				{ "cyan", 0, 255, 255 },
				{ "brown", 165, 42, 42 },
				{ "transparent", 0, 0, 0 },
			};
			for ( const auto& c : k_colors )
			{
				push_color( L, c.r, c.g, c.b, 255 );
				lua_setfield( L, -2, c.name );
			}

			lua_setglobal( L, "color" );

			lua_newtable( L );
			{
				lua_pushcfunction( L, l_globals_index );
				lua_setfield( L, -2, "__index" );
				lua_setmetatable( L, -2 );
			}
			lua_setglobal( L, "globals" );

			lua_newtable( L );
			{
				push_method( L, "screen_size", l_render_screen_size );
				push_method( L, "camera_position", l_render_camera_position );
				push_method( L, "camera_angles", l_render_camera_angles );
				push_method( L, "world_to_screen", l_render_world_to_screen );
				push_method( L, "get_offscreen", l_render_get_offscreen );
				push_method( L, "measure_text", l_render_measure_text );
				push_method( L, "rect", l_render_rect );
				push_method( L, "rect_outline", l_render_rect_outline );
				push_method( L, "gradient", l_render_gradient );
				push_method( L, "line", l_render_line );
				push_method( L, "circle", l_render_circle );
				push_method( L, "circle_outline", l_render_circle_outline );
				push_method( L, "circle_3d", l_render_circle_3d );
				push_method( L, "circle_3d_outline", l_render_circle_3d_outline );
				push_method( L, "text", l_render_text );
				push_method( L, "push_clip_rect", l_render_push_clip );
				push_method( L, "pop_clip_rect", l_render_pop_clip );
			}
			lua_setglobal( L, "render" );

			lua_newtable( L );
			{
				lua_pushcfunction( L, l_cvar_index );
				lua_setfield( L, -2, "__index" );
				lua_setmetatable( L, -2 );
			}
			lua_setglobal( L, "cvar" );

			lua_newtable( L );
			{
				push_method( L, "get", l_entity_get );
				push_method( L, "get_local_player", l_entity_get_local_player );
				push_method( L, "get_players", l_entity_get_players );
			}
			lua_setglobal( L, "entity" );

			lua_newtable( L );
			{
				lua_pushcfunction( L, l_events_index );
				lua_setfield( L, -2, "__index" );
				lua_setmetatable( L, -2 );
			}
			lua_setglobal( L, "events" );

			lua_newtable( L );
			{
				push_method( L, "log", l_client_log );
				push_method( L, "get_latency", l_client_get_latency );
				push_method( L, "get_name", l_client_get_name );
			}
			lua_setglobal( L, "client" );
		}

		[[nodiscard]] std::filesystem::path scripts_directory( )
		{
			const auto module = game_path::module_path( nullptr );
			if ( module )
			{
				return module->parent_path( ) / "scripts";
			}

			return std::filesystem::path( "scripts" );
		}

	}


	static constexpr bool k_lua_disabled{ false };

	void lua::ensure_state( )
	{
		if constexpr ( k_lua_disabled )
		{
			this->m_initialized = false;
			this->m_error = "lua engine disabled";
			return;
		}

		if ( this->m_state )
		{
			return;
		}

		diag::write( diag::level::info, "[lua] creating lua state" );
		this->m_state = luaL_newstate( );
		if ( !this->m_state )
		{
			diag::write( diag::level::error, "[lua] luaL_newstate returned nullptr" );
			this->m_error = "failed to create lua state";
			this->m_initialized = false;
			return;
		}

		diag::write( diag::level::debug, "[lua] opening standard libraries" );
		try
		{
			luaL_openlibs( this->m_state );
		}
		catch ( const std::exception& e )
		{
			diag::writef( diag::level::error, "[lua] luaL_openlibs threw: %s", e.what( ) );
			lua_close( this->m_state );
			this->m_state = nullptr;
			this->m_initialized = false;
			this->m_error = std::string( "luaL_openlibs failed: " ) + e.what( );
			return;
		}
		catch ( ... )
		{
			diag::write( diag::level::error, "[lua] luaL_openlibs threw unknown exception" );
			lua_close( this->m_state );
			this->m_state = nullptr;
			this->m_initialized = false;
			this->m_error = "luaL_openlibs failed";
			return;
		}

		diag::write( diag::level::debug, "[lua] binding api" );
		try
		{
			this->bind_api( this->m_state );
		}
		catch ( const std::exception& e )
		{
			diag::writef( diag::level::error, "[lua] bind_api threw: %s", e.what( ) );
			lua_close( this->m_state );
			this->m_state = nullptr;
			this->m_initialized = false;
			this->m_error = std::string( "bind_api failed: " ) + e.what( );
			return;
		}
		catch ( ... )
		{
			diag::write( diag::level::error, "[lua] bind_api threw unknown exception" );
			lua_close( this->m_state );
			this->m_state = nullptr;
			this->m_initialized = false;
			this->m_error = "bind_api failed";
			return;
		}

		diag::write( diag::level::debug, "[lua] registering legacy api table" );
		lua_newtable( this->m_state );
		luaL_setfuncs( this->m_state, k_velo_funcs, 0 );
			lua_setglobal( this->m_state, "aetheris" );

		this->m_initialized = true;
		diag::write( diag::level::info, "[lua] state initialized" );
	}

	void lua::bind_api( lua_State* state )
	{
		if constexpr ( k_lua_disabled )
		{
			return;
		}

		diag::write( diag::level::debug, "[lua] register_types begin" );
		register_types( state );
		diag::write( diag::level::debug, "[lua] register_types end" );
	}

	std::vector<std::string> lua::script_names( ) const
	{
		std::lock_guard lock{ this->m_mutex };

		std::vector<std::string> names{};
		names.reserve( this->m_scripts.size( ) );
		for ( const auto& s : this->m_scripts )
		{
			names.push_back( s.path.filename( ).string( ) );
		}
		return names;
	}

	std::string lua::scripts_directory_name( ) const
	{
		try
		{
			return scripts_directory( ).string( );
		}
		catch ( ... )
		{
			return "scripts";
		}
	}

	void lua::reload( )
	{
		if constexpr ( k_lua_disabled )
		{
			this->m_scripts.clear( );
			this->m_reload_requested = false;
			this->m_initialized = false;
			return;
		}

		diag::write( diag::level::info, "[lua] reload begin" );

		if ( this->m_state )
		{
			diag::write( diag::level::debug, "[lua] closing existing state" );
			lua_close( this->m_state );
			this->m_state = nullptr;
			this->m_initialized = false;
			g_event_refs.clear( );
		}

		this->ensure_state( );
		if ( !this->m_state )
		{
			diag::write( diag::level::error, "[lua] reload aborted: state is null" );
			return;
		}

		std::error_code ec{};
		std::filesystem::path dir{};

		try
		{
			dir = scripts_directory( );
		}
		catch ( ... )
		{
			diag::write( diag::level::error, "[lua] scripts_directory threw" );
			this->m_scripts.clear( );
			this->m_reload_requested = false;
			return;
		}

		diag::writef( diag::level::debug, "[lua] scripts dir: %s", dir.string( ).c_str( ) );

		std::filesystem::create_directories( dir, ec );
		if ( ec )
		{
			diag::writef( diag::level::warning, "[lua] create_directories failed: %s", ec.message( ).c_str( ) );
			ec.clear( );
		}

		std::vector<script> scripts{};

		try
		{
			if ( !std::filesystem::exists( dir, ec ) || ec )
			{
				diag::write( diag::level::warning, "[lua] scripts dir missing after create" );
				this->m_scripts.clear( );
				this->m_reload_requested = false;
				return;
			}

			std::filesystem::directory_iterator it{ dir, std::filesystem::directory_options::skip_permission_denied, ec };
			if ( ec )
			{
				diag::writef( diag::level::error, "[lua] directory_iterator failed: %s", ec.message( ).c_str( ) );
				this->m_scripts.clear( );
				this->m_reload_requested = false;
				return;
			}

			for ( const auto& entry : it )
			{
				std::error_code entry_ec{};
				if ( !entry.is_regular_file( entry_ec ) || entry_ec )
				{
					continue;
				}

				if ( entry.path( ).extension( ) != ".lua" )
				{
					continue;
				}

				diag::writef( diag::level::debug, "[lua] found script: %s", entry.path( ).filename( ).string( ).c_str( ) );
				scripts.push_back( script{ entry.path( ), entry.last_write_time( entry_ec ) } );
			}
		}
		catch ( const std::exception& e )
		{
			diag::writef( diag::level::error, "[lua] script scan threw: %s", e.what( ) );
			this->m_error = std::string( "script scan failed: " ) + e.what( );
		}
		catch ( ... )
		{
			diag::write( diag::level::error, "[lua] script scan threw unknown exception" );
			this->m_error = "script scan failed";
		}

		diag::writef( diag::level::info, "[lua] reload done, %d scripts", static_cast< int >( scripts.size( ) ) );
		this->m_scripts = std::move( scripts );
		this->m_reload_requested = false;
	}

	bool lua::needs_reload( ) const
	{
		if constexpr ( k_lua_disabled )
		{
			return false;
		}

		const auto now = std::chrono::steady_clock::now( );
		if ( now - this->m_last_scan < std::chrono::milliseconds( 500 ) )
		{
			return false;
		}
		this->m_last_scan = now;

		std::error_code ec{};
		std::filesystem::path dir{};
		try
		{
			dir = scripts_directory( );
		}
		catch ( ... )
		{
			return false;
		}

		std::size_t count{};
		try
		{
			std::filesystem::directory_iterator it{ dir, std::filesystem::directory_options::skip_permission_denied, ec };
			if ( ec )
			{
				return false;
			}

			for ( const auto& entry : it )
			{
				if ( entry.path( ).extension( ) == ".lua" )
				{
					++count;
				}
			}
		}
		catch ( ... )
		{
			return false;
		}

		if ( count != this->m_scripts.size( ) )
		{
			return true;
		}

		for ( const auto& script : this->m_scripts )
		{
			std::error_code time_ec{};
			const auto mtime = std::filesystem::last_write_time( script.path, time_ec );
			if ( time_ec )
			{
				continue;
			}
			if ( mtime != script.mtime )
			{
				return true;
			}
		}

		return false;
	}

	void lua::on_render( xdraw::draw_list& draw_list )
	{
		if constexpr ( k_lua_disabled )
		{
			return;
		}

		if ( !settings::g_misc.m_lua.enabled.value )
		{
			return;
		}

		std::lock_guard lock{ this->m_mutex };

		if ( !this->m_initialized )
		{
			this->ensure_state( );
			if ( !this->m_state )
			{
				return;
			}
			this->reload( );
		}

		if ( !this->m_state )
		{
			return;
		}

		if ( this->m_reload_requested || this->needs_reload( ) )
		{
			this->reload( );
			if ( !this->m_state )
			{
				return;
			}
		}

		current_paint( ).draw_list = &draw_list;
		current_paint( ).pawn = systems::g_local.get( ).pawn;

		diag::write( diag::level::debug, "[lua] on_paint begin" );

		for ( auto& script : this->m_scripts )
		{
			if ( script.loaded )
			{
				continue;
			}

			if ( !this->is_script_enabled( script.path.filename( ).string( ) ) )
			{
				continue;
			}

			const auto status = luaL_loadfile( this->m_state, script.path.string( ).c_str( ) );
			if ( status != LUA_OK )
			{
				const auto* msg = lua_tostring( this->m_state, -1 );
				this->m_error = msg ? msg : "unknown load error";
				diag::writef( diag::level::error, "[lua] load failed (%s): %s",
					script.path.filename( ).string( ).c_str( ), this->m_error.c_str( ) );
				lua_pop( this->m_state, 1 );
				script.loaded = true;
				continue;
			}

			if ( lua_pcall( this->m_state, 0, 0, 0 ) != LUA_OK )
			{
				const auto* msg = lua_tostring( this->m_state, -1 );
				this->m_error = msg ? msg : "unknown runtime error";
				diag::writef( diag::level::error, "[lua] exec failed (%s): %s",
					script.path.filename( ).string( ).c_str( ), this->m_error.c_str( ) );
				lua_pop( this->m_state, 1 );
				script.loaded = true;
				continue;
			}

			script.loaded = true;
		}

		lua_getglobal( this->m_state, "on_paint" );
		if ( lua_isfunction( this->m_state, -1 ) )
		{
			if ( lua_pcall( this->m_state, 0, 0, 0 ) != LUA_OK )
			{
				const auto* msg = lua_tostring( this->m_state, -1 );
				this->m_error = msg ? msg : "unknown on_paint error";
				lua_pop( this->m_state, 1 );
			}
		}
		else
		{
			lua_pop( this->m_state, 1 );
		}

		fire_event( this->m_state, "paint" );

		current_paint( ).draw_list = nullptr;
		current_paint( ).pawn = 0;

		diag::write( diag::level::debug, "[lua] on_paint end" );
	}

	void lua::on_create_move( systems::input::usercmd* cmd )
	{
		if constexpr ( k_lua_disabled )
		{
			return;
		}

		if ( !settings::g_misc.m_lua.enabled.value )
		{
			return;
		}

		std::lock_guard lock{ this->m_mutex };

		if ( !this->m_state || !this->m_initialized )
		{
			return;
		}

		fire_event( this->m_state, "createmove" );

		lua_getglobal( this->m_state, "on_createmove" );
		if ( lua_isfunction( this->m_state, -1 ) )
		{
			if ( lua_pcall( this->m_state, 0, 0, 0 ) != LUA_OK )
			{
				const auto* msg = lua_tostring( this->m_state, -1 );
				this->m_error = msg ? msg : "unknown on_createmove error";
				diag::writef( diag::level::error, "[lua] on_createmove: %s", this->m_error.c_str( ) );
				lua_pop( this->m_state, 1 );
			}
		}
		else
		{
			lua_pop( this->m_state, 1 );
		}
	}

}