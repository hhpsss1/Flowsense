





#ifndef SOL_ABORT_HPP
#define SOL_ABORT_HPP

#include <sol/version.hpp>

#include <sol/base_traits.hpp>

#include <cstdlib>

#if SOL_IS_ON(SOL_DEBUG_BUILD)
	#if SOL_IS_ON(SOL_COMPILER_VCXX)
		#define SOL_DEBUG_ABORT() \
			if (true) { ::std::abort(); } \
			static_assert(true, "")
	#else
		#define SOL_DEBUG_ABORT() ::std::abort()
	#endif
#else
	#define SOL_DEBUG_ABORT() static_assert(true, "")
#endif

#endif
