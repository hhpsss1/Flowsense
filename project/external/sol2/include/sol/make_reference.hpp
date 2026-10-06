





#ifndef SOL_MAKE_REFERENCE_HPP
#define SOL_MAKE_REFERENCE_HPP

#include <sol/reference.hpp>
#include <sol/stack.hpp>

namespace sol {

	template <typename R = reference, bool should_pop = !is_stack_based_v<R>, typename T>
	R make_reference(lua_State* L, T&& value) {
		int backpedal = stack::push(L, std::forward<T>(value));
		R r = stack::get<R>(L, -backpedal);
		if (should_pop) {
			lua_pop(L, backpedal);
		}
		return r;
	}

	template <typename T, typename R = reference, bool should_pop = !is_stack_based_v<R>, typename... Args>
	R make_reference(lua_State* L, Args&&... args) {
		int backpedal = stack::push<T>(L, std::forward<Args>(args)...);
		R r = stack::get<R>(L, -backpedal);
		if (should_pop) {
			lua_pop(L, backpedal);
		}
		return r;
	}

	template <typename R = reference, bool should_pop = !is_stack_based_v<R>, typename T>
	R make_reference_userdata(lua_State* L, T&& value) {
		int backpedal = stack::push_userdata(L, std::forward<T>(value));
		R r = stack::get<R>(L, -backpedal);
		if (should_pop) {
			lua_pop(L, backpedal);
		}
		return r;
	}

	template <typename T, typename R = reference, bool should_pop = !is_stack_based_v<R>, typename... Args>
	R make_reference_userdata(lua_State* L, Args&&... args) {
		int backpedal = stack::push_userdata<T>(L, std::forward<Args>(args)...);
		R r = stack::get<R>(L, -backpedal);
		if (should_pop) {
			lua_pop(L, backpedal);
		}
		return r;
	}

}

#endif
