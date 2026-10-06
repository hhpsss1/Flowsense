





#pragma once

#ifndef SOL_ASSERT_HPP
#define SOL_ASSERT_HPP

#include <sol/forward.hpp>

#if SOL_IS_ON(SOL2_CI)

struct pre_main {
	pre_main() {
#ifdef _MSC_VER
		_set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif
	}
} inline sol2_ci_dont_lock_ci_please = {};

#endif



#if SOL_IS_ON(SOL_USER_ASSERT)
	#define SOL_ASSERT(...) SOL_C_ASSERT(__VA_ARGS__)
#else
	#if SOL_IS_ON(SOL_DEBUG_BUILD)
		#include <exception>
		#include <iostream>
		#include <cstdlib>

			#define SOL_ASSERT(...)                                                                                               \
				do {                                                                                                               \
					if (!(__VA_ARGS__)) {                                                                                           \
						std::cerr << "Assertion `" #__VA_ARGS__ "` failed in " << __FILE__ << " line " << __LINE__ << std::endl; \
						std::terminate();                                                                                        \
					}                                                                                                             \
				} while (false)
	#else
		#define SOL_ASSERT(...)           \
			do {                           \
				if (false) {              \
					(void)(__VA_ARGS__); \
				}                         \
			} while (false)
	#endif
#endif

#if SOL_IS_ON(SOL_USER_ASSERT_MSG)
	#define SOL_ASSERT_MSG(message, ...) SOL_ASSERT_MSG(message, __VA_ARGS__)
#else
	#if SOL_IS_ON(SOL_DEBUG_BUILD)
		#include <exception>
		#include <iostream>
		#include <cstdlib>

		#define SOL_ASSERT_MSG(message, ...)                                                                                                         \
			do {                                                                                                                                  \
				if (!(__VA_ARGS__)) {                                                                                                              \
					std::cerr << "Assertion `" #__VA_ARGS__ "` failed in " << __FILE__ << " line " << __LINE__ << ": " << message << std::endl; \
					std::terminate();                                                                                                           \
				}                                                                                                                                \
			} while (false)
	#else
		#define SOL_ASSERT_MSG(message, ...)    \
			do {                             \
				if (false) {                \
					(void)(__VA_ARGS__);   \
					(void)sizeof(message); \
				}                           \
			} while (false)
	#endif
#endif


#endif
