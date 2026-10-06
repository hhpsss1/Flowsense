





#pragma once


#if defined(SOL_PROLOGUE_I_)
	#error "[sol2] Library Prologue was already included in translation unit and not properly ended with an epilogue."
#endif

#define SOL_PROLOGUE_I_ 1


#if SOL_IS_ON(SOL_BUILD_CXX_MODE)
	#define _FWD(...) static_cast<decltype( __VA_ARGS__ )&&>( __VA_ARGS__ )

	#if SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG)
		#define _MOVE(...) static_cast<__typeof( __VA_ARGS__ )&&>( __VA_ARGS__ )
	#else
		#include <type_traits>
		
		#define _MOVE(...) static_cast<::std::remove_reference_t<( __VA_ARGS__ )>&&>( __VA_OPT__(,) )
	#endif
#endif

