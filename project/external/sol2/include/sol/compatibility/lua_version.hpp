





#ifndef SOL_COMPATIBILITY_LUA_VERSION_HPP
#define SOL_COMPATIBILITY_LUA_VERSION_HPP

#include <sol/version.hpp>


#if SOL_IS_ON(SOL_USING_CXX_LUA)
	#if __has_include(<lua/lua.h>)
		#include <lua/lua.h>
		#include <lua/lauxlib.h>
		#include <lua/lualib.h>
	#else
		#include <lua.h>
		#include <lauxlib.h>
		#include <lualib.h>
	#endif
#elif SOL_IS_ON(SOL_USE_LUA_HPP)
	#if __has_include(<lua/lua.hpp>)
		#include <lua/lua.hpp>
	#else
		#include <lua.hpp>
	#endif
#else
	extern "C" {
		#if __has_include(<lua/lua.h>)
			#include <lua/lua.h>
			#include <lua/lauxlib.h>
			#include <lua/lualib.h>
		#else
			#include <lua.h>
			#include <lauxlib.h>
			#include <lualib.h>
		#endif
	}
#endif

#if defined(SOL_LUAJIT)
	#if (SOL_LUAJIT != 0)
		#define SOL_USE_LUAJIT_I_ SOL_ON
	#else
		#define SOL_USE_LUAJIT_I_ SOL_OFF
	#endif
#elif defined(LUAJIT_VERSION)
	#define SOL_USE_LUAJIT_I_ SOL_ON
#elif SOL_IS_ON(SOL_USING_CXX_LUAJIT)
	#define SOL_USE_LUAJIT_I_ SOL_ON
#else
	#define SOL_USE_LUAJIT_I_ SOL_DEFAULT_OFF
#endif

#if SOL_IS_ON(SOL_USING_CXX_LUAJIT)
	#include <luajit.h>
#elif SOL_IS_ON(SOL_USE_LUAJIT)
	extern "C" {
		#include <luajit.h>
	}
#endif

#if defined(SOL_LUAJIT_VERSION)
	#define SOL_LUAJIT_VERSION_I_ SOL_LUAJIT_VERSION
#elif SOL_IS_ON(SOL_USE_LUAJIT)
	#define SOL_LUAJIT_VERSION_I_ LUAJIT_VERSION_NUM
#else
	#define SOL_LUAJIT_VERSION_I_ 0
#endif

#if defined(SOL_LUAJIT_FFI_DISABLED)
	#define SOL_LUAJIT_FFI_DISABLED_I_ SOL_ON
#elif defined(LUAJIT_DISABLE_FFI)
	#define SOL_LUAJIT_FFI_DISABLED_I_ SOL_ON
#else
	#define SOL_LUAJIT_FFI_DISABLED_I_ SOL_DEFAULT_OFF
#endif

#if defined(MOONJIT_VERSION)
	#define SOL_USE_MOONJIT_I_ SOL_ON
#else
	#define SOL_USE_MOONJIT_I_ SOL_OFF
#endif

#if !defined(SOL_LUA_VERSION)
	#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 502
		#define SOL_LUA_VERSION LUA_VERSION_NUM
	#elif defined(LUA_VERSION_NUM) && LUA_VERSION_NUM == 501
		#define SOL_LUA_VERSION LUA_VERSION_NUM
	#elif !defined(LUA_VERSION_NUM) || !(LUA_VERSION_NUM)
		#define SOL_LUA_VERSION 500
	#else
		#define SOL_LUA_VERSION 504
	#endif
#endif

#if defined(SOL_LUA_VERSION)
	#define SOL_LUA_VERSION_I_ SOL_LUA_VERSION
#else
	#define SOL_LUA_VERSION_I_ 504
#endif

#if defined(SOL_EXCEPTIONS_ALWAYS_UNSAFE)
	#if (SOL_EXCEPTIONS_ALWAYS_UNSAFE != 0)
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_OFF
	#else
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_ON
	#endif
#elif defined(SOL_EXCEPTIONS_SAFE_PROPAGATION)
	#if (SOL_EXCEPTIONS_SAFE_PROPAGATION != 0)
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_ON
	#else
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_OFF
	#endif
#else
	#if SOL_IS_ON(SOL_USE_LUAJIT)
		#if SOL_USE(SOL_LUAJIT_VERSION) >= 20100
			#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_ON
		#elif SOL_USE(SOL_LUAJIT_VERSION) >= 20000
			#if SOL_IS_ON(SOL_PLATFORM_X64)
				#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_ON
			#else
				#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_OFF
			#endif
		#endif
	#else
		#define SOL_PROPAGATE_EXCEPTIONS_I_ SOL_DEFAULT_OFF
	#endif
#endif

#if defined(SOL_EXCEPTIONS_CATCH_ALL)
	#if (SOL_EXCEPTIONS_CATCH_ALL != 0)
		#define SOL_EXCEPTIONS_CATCH_ALL_I_ SOL_ON
	#else
		#define SOL_EXCEPTIONS_CATCH_ALL_I_ SOL_OFF
	#endif
#else
	#if SOL_IS_ON(SOL_USE_LUAJIT) || SOL_IS_ON(SOL_USING_CXX_LUAJIT)
		#define SOL_EXCEPTIONS_CATCH_ALL_I_ SOL_DEFAULT_OFF
	#elif SOL_IS_ON(SOL_USING_CXX_LUA)
		#define SOL_EXCEPTIONS_CATCH_ALL_I_ SOL_DEFAULT_OFF
	#else
		#define SOL_EXCEPTIONS_CATCH_ALL_I_ SOL_DEFAULT_ON
	#endif
#endif

#if defined(SOL_LUAJIT_USE_EXCEPTION_TRAMPOLINE)
	#if (SOL_LUAJIT_USE_EXCEPTION_TRAMPOLINE != 0)
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_ON
	#else
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_OFF
	#endif
#else
	#if SOL_IS_OFF(SOL_PROPAGATE_EXCEPTIONS) && SOL_IS_ON(SOL_USE_LUAJIT)
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_ON
	#else
		#define SOL_USE_LUAJIT_EXCEPTION_TRAMPOLINE_I_ SOL_DEFAULT_OFF
	#endif
#endif

#if defined(SOL_LUAL_STREAM_HAS_CLOSE_FUNCTION)
	#if (SOL_LUAL_STREAM_HAS_CLOSE_FUNCTION != 0)
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_ON
	#else
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_OFF
	#endif
#else
	#if SOL_IS_OFF(SOL_USE_LUAJIT) && (SOL_LUA_VERSION > 501)
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_ON
	#else
		#define SOL_LUAL_STREAM_USE_CLOSE_FUNCTION_I_ SOL_DEFAULT_OFF
	#endif
#endif

#if defined (SOL_LUA_BIT32_LIB)
	#if SOL_LUA_BIT32_LIB != 0
		#define SOL_LUA_BIT32_LIB_I_ SOL_ON
	#else
		#define SOL_LUA_BIT32_LIB_I_ SOL_OFF
	#endif
#else
	#if (SOL_LUA_VERSION_I_ == 502)
		#define SOL_LUA_BIT32_LIB_I_ SOL_ON
	#elif defined(LUA_COMPAT_BITLIB)
		#define SOL_LUA_BIT32_LIB_I_ SOL_ON
	#elif (SOL_LUA_VERSION_I_ < 504 && defined(LUA_COMPAT_5_2))
		#define SOL_LUA_BIT32_LIB_I_ SOL_ON
	#else
		#define SOL_LUA_BIT32_LIB_I_ SOL_DEFAULT_OFF
	#endif
#endif

#if defined (SOL_LUA_NIL_IN_TABLES)
	#if SOL_LUA_NIL_IN_TABLES != 0
		#define SOL_LUA_NIL_IN_TABLES_I_ SOL_ON
	#else
		#define SOL_LUA_NIL_IN_TABLES_I_ SOL_OFF
	#endif
#else
	#if defined(LUA_NILINTABLE) && (LUA_NILINTABLE != 0)
		#define SOL_LUA_NIL_IN_TABLES_I_ SOL_DEFAULT_ON
	#else
		#define SOL_LUA_NIL_IN_TABLES_I_ SOL_DEFAULT_OFF
	#endif
#endif


#endif
