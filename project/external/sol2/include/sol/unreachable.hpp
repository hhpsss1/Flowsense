





#ifndef SOL_UNREACHABLE_HPP
#define SOL_UNREACHABLE_HPP

#include <sol/version.hpp>

#if SOL_HAS_BUILTIN_I_(__builtin_unreachable)
#define SOL_UNREACHABLE() __builtin_unreachable();
#elif SOL_IS_ON(SOL_COMPILER_VCXX)
#define SOL_UNREACHABLE() __assume(false);
#else
#define SOL_UNREACHABLE() __builtin_unreachable();
#endif

#endif
