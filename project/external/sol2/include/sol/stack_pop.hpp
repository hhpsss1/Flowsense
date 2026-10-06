





#ifndef SOL_STACK_POP_HPP
#define SOL_STACK_POP_HPP

#include <sol/stack_core.hpp>
#include <sol/stack_get.hpp>
#include <utility>
#include <tuple>

namespace sol { namespace stack {
	template <typename T, typename>
	struct popper {
		inline static decltype(auto) pop(lua_State* L) {
			if constexpr (is_stack_based_v<meta::unqualified_t<T>>) {
				static_assert(!is_stack_based_v<meta::unqualified_t<T>>,
					"You cannot pop something that lives solely on the stack: it will not remain on the stack when popped and thusly will go out of "
					"scope!");
			}
			else {
				record tracking {};
				decltype(auto) r = get<T>(L, -lua_size<T>::value, tracking);
				lua_pop(L, tracking.used);
				return r;
			}
		}
	};
}}

#endif
