





#pragma once

#ifndef SOL_DETAIL_BUILD_VERSION_HPP
#define SOL_DETAIL_BUILD_VERSION_HPP

#include <sol/version.hpp>


#if defined(SOL_DLL)
	#if (SOL_DLL != 0)
		#define SOL_DLL_I_ SOL_ON
	#else
		#define SOL_DLL_I_ SOL_OFF
	#endif
#elif SOL_IS_ON(SOL_COMPILER_VCXX) && (defined(DLL_) || defined(_DLL))
	#define SOL_DLL_I_ SOL_DEFAULT_ON
#else
	#define SOL_DLL_I_ SOL_DEFAULT_OFF
#endif

#if defined(SOL_HEADER_ONLY)
	#if (SOL_HEADER_ONLY != 0)
		#define SOL_HEADER_ONLY_I_ SOL_ON
	#else
		#define SOL_HEADER_ONLY_I_ SOL_OFF
	#endif
#else
	#define SOL_HEADER_ONLY_I_ SOL_DEFAULT_OFF
#endif

#if defined(SOL_BUILD)
	#if (SOL_BUILD != 0)
		#define SOL_BUILD_I_ SOL_ON
	#else
		#define SOL_BUILD_I_ SOL_OFF
	#endif
#elif SOL_IS_ON(SOL_HEADER_ONLY)
	#define SOL_BUILD_I_ SOL_DEFAULT_OFF
#else
	#define SOL_BUILD_I_ SOL_DEFAULT_ON
#endif

#if defined(SOL_UNITY_BUILD)
	#if (SOL_UNITY_BUILD != 0)
		#define SOL_UNITY_BUILD_I_ SOL_ON
	#else
		#define SOL_UNITY_BUILD_I_ SOL_OFF
	#endif
#else
	#define SOL_UNITY_BUILD_I_ SOL_DEFAULT_OFF
#endif

#if defined(SOL_C_FUNCTION_LINKAGE)
	#define SOL_C_FUNCTION_LINKAGE_I_ SOL_C_FUNCTION_LINKAGE
#else
	#if SOL_IS_ON(SOL_BUILD_CXX_MODE)
		#define SOL_C_FUNCTION_LINKAGE_I_ extern "C"
	#else
		#define SOL_C_FUNCTION_LINKAGE_I_
	#endif
#endif

#if defined(SOL_API_LINKAGE)
	#define SOL_API_LINKAGE_I_ SOL_API_LINKAGE
#else
	#if SOL_IS_ON(SOL_DLL)
		#if SOL_IS_ON(SOL_COMPILER_VCXX) || SOL_IS_ON(SOL_PLATFORM_WINDOWS) || SOL_IS_ON(SOL_PLATFORM_CYGWIN)
			#if SOL_IS_ON(SOL_BUILD)
				#if SOL_IS_ON(SOL_COMPILER_GCC)
					#define SOL_API_LINKAGE_I_ __attribute__((dllexport))
				#else
					#define SOL_API_LINKAGE_I_ __declspec(dllexport)
				#endif
			#else
				#if SOL_IS_ON(SOL_COMPILER_GCC)
					#define SOL_API_LINKAGE_I_ __attribute__((dllimport))
				#else
					#define SOL_API_LINKAGE_I_ __declspec(dllimport)
				#endif
			#endif
		#else
			#define SOL_API_LINKAGE_I_ extern
		#endif
	#elif SOL_IS_ON(SOL_UNITY_BUILD)
		#if SOL_IS_ON(SOL_HEADER_ONLY)
			#define SOL_API_LINKAGE_I_ inline
		#else
			#define SOL_API_LINKAGE_I_ extern
		#endif
	#else
		#if SOL_IS_ON(SOL_BUILD_CXX_MODE)
			#define SOL_API_LINKAGE_I_
		#else
			#define SOL_API_LINKAGE_I_ extern
		#endif
	#endif
#endif

#if defined(SOL_PUBLIC_FUNC_DECL)
	#define SOL_PUBLIC_FUNC_DECL_I_ SOL_PUBLIC_FUNC_DECL
#else
	#define SOL_PUBLIC_FUNC_DECL_I_ SOL_API_LINKAGE_I_
#endif

#if defined(SOL_INTERNAL_FUNC_DECL_)
	#define SOL_INTERNAL_FUNC_DECL_I_ SOL_INTERNAL_FUNC_DECL_
#else
	#define SOL_INTERNAL_FUNC_DECL_I_ SOL_API_LINKAGE_I_
#endif

#if defined(SOL_PUBLIC_FUNC_DEF)
	#define SOL_PUBLIC_FUNC_DEF_I_ SOL_PUBLIC_FUNC_DEF
#else
	#define SOL_PUBLIC_FUNC_DEF_I_ SOL_API_LINKAGE_I_
#endif

#if defined(SOL_INTERNAL_FUNC_DEF)
	#define SOL_INTERNAL_FUNC_DEF_I_ SOL_INTERNAL_FUNC_DEF
#else
	#define SOL_INTERNAL_FUNC_DEF_I_ SOL_API_LINKAGE_I_
#endif

#if defined(SOL_FUNC_DECL)
	#define SOL_FUNC_DECL_I_ SOL_FUNC_DECL
#elif SOL_IS_ON(SOL_HEADER_ONLY)
	#define SOL_FUNC_DECL_I_ 
#elif SOL_IS_ON(SOL_DLL)
	#if SOL_IS_ON(SOL_COMPILER_VCXX)
		#if SOL_IS_ON(SOL_BUILD)
			#define SOL_FUNC_DECL_I_ extern __declspec(dllexport)
		#else
			#define SOL_FUNC_DECL_I_ extern __declspec(dllimport)
		#endif
	#elif SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG)
		#define SOL_FUNC_DECL_I_ extern __attribute__((visibility("default")))
	#else
		#define SOL_FUNC_DECL_I_ extern
	#endif
#endif

#if defined(SOL_FUNC_DEFN)
	#define SOL_FUNC_DEFN_I_ SOL_FUNC_DEFN
#elif SOL_IS_ON(SOL_HEADER_ONLY)
	#define SOL_FUNC_DEFN_I_ inline
#elif SOL_IS_ON(SOL_DLL)
	#if SOL_IS_ON(SOL_COMPILER_VCXX)
		#if SOL_IS_ON(SOL_BUILD)
			#define SOL_FUNC_DEFN_I_ __declspec(dllexport)
		#else
			#define SOL_FUNC_DEFN_I_ __declspec(dllimport)
		#endif
	#elif SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG)
		#define SOL_FUNC_DEFN_I_ __attribute__((visibility("default")))
	#else
		#define SOL_FUNC_DEFN_I_
	#endif
#endif

#if defined(SOL_HIDDEN_FUNC_DECL)
	#define SOL_HIDDEN_FUNC_DECL_I_ SOL_HIDDEN_FUNC_DECL
#elif SOL_IS_ON(SOL_HEADER_ONLY)
	#define SOL_HIDDEN_FUNC_DECL_I_ 
#elif SOL_IS_ON(SOL_DLL)
	#if SOL_IS_ON(SOL_COMPILER_VCXX)
		#if SOL_IS_ON(SOL_BUILD)
			#define SOL_HIDDEN_FUNC_DECL_I_ extern __declspec(dllexport)
		#else
			#define SOL_HIDDEN_FUNC_DECL_I_ extern __declspec(dllimport)
		#endif
	#elif SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG)
		#define SOL_HIDDEN_FUNC_DECL_I_ extern __attribute__((visibility("default")))
	#else
		#define SOL_HIDDEN_FUNC_DECL_I_ extern
	#endif
#endif

#if defined(SOL_HIDDEN_FUNC_DEFN)
	#define SOL_HIDDEN_FUNC_DEFN_I_ SOL_HIDDEN_FUNC_DEFN
#elif SOL_IS_ON(SOL_HEADER_ONLY)
	#define SOL_HIDDEN_FUNC_DEFN_I_ inline
#elif SOL_IS_ON(SOL_DLL)
	#if SOL_IS_ON(SOL_COMPILER_VCXX)
		#if SOL_IS_ON(SOL_BUILD)
			#define SOL_HIDDEN_FUNC_DEFN_I_ 
		#else
			#define SOL_HIDDEN_FUNC_DEFN_I_ 
		#endif
	#elif SOL_IS_ON(SOL_COMPILER_GCC) || SOL_IS_ON(SOL_COMPILER_CLANG)
		#define SOL_HIDDEN_FUNC_DEFN_I_ __attribute__((visibility("hidden")))
	#else
		#define SOL_HIDDEN_FUNC_DEFN_I_
	#endif
#endif


#endif
