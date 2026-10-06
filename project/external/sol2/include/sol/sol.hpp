




#ifndef SOL_HPP
#define SOL_HPP

#include <sol/version.hpp>

#if SOL_IS_ON(SOL_INSIDE_UNREAL_ENGINE)
#ifdef check
#pragma push_macro("check")
#undef check
#endif
#endif

#if SOL_IS_ON(SOL_COMPILER_GCC)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#pragma GCC diagnostic ignored "-Wconversion"
#if __GNUC__ > 6
#pragma GCC diagnostic ignored "-Wnoexcept-type"
#endif
#elif SOL_IS_ON(SOL_COMPILER_CLANG)
#elif SOL_IS_ON(SOL_COMPILER_VCXX)
#pragma warning(push)
#pragma warning(disable : 4505)
#endif

#include <sol/forward.hpp>
#include <sol/forward_detail.hpp>
#include <sol/assert.hpp>
#include <sol/bytecode.hpp>
#include <sol/stack.hpp>
#include <sol/object.hpp>
#include <sol/function.hpp>
#include <sol/protected_function.hpp>
#include <sol/usertype.hpp>
#include <sol/table.hpp>
#include <sol/state.hpp>
#include <sol/coroutine.hpp>
#include <sol/thread.hpp>
#include <sol/userdata.hpp>
#include <sol/metatable.hpp>
#include <sol/as_args.hpp>
#include <sol/variadic_args.hpp>
#include <sol/variadic_results.hpp>
#include <sol/lua_value.hpp>

#if SOL_IS_ON(SOL_COMPILER_GCC)
#pragma GCC diagnostic pop
#elif SOL_IS_ON(SOL_COMPILER_CLANG)
#elif SOL_IS_ON(SOL_COMPILER_VCXX)
#pragma warning(pop)
#endif

#if SOL_IS_ON(SOL_INSIDE_UNREAL_ENGINE)
#undef check
#pragma pop_macro("check")
#endif

#endif
