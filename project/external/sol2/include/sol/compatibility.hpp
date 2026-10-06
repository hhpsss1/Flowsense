





#ifndef SOL_COMPATIBILITY_HPP
#define SOL_COMPATIBILITY_HPP


#include <sol/version.hpp>
#include <sol/compatibility/lua_version.hpp>

#if SOL_IS_ON(SOL_USE_COMPATIBILITY_LAYER)

#if SOL_IS_ON(SOL_USING_CXX_LUA) || SOL_IS_ON(SOL_USING_CXX_LUAJIT)
	#ifndef COMPAT53_LUA_CPP
		#define COMPAT53_LUA_CPP 1
	#endif
#endif
	#ifndef COMPAT53_INCLUDE_SOURCE
		#define COMPAT53_INCLUDE_SOURCE 1
	#endif
	#include <sol/compatibility/compat-5.3.h>
	#include <sol/compatibility/compat-5.4.h>
#endif

#endif
