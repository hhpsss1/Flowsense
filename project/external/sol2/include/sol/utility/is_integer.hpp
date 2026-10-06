





#ifndef SOL_IS_INTEGER_HPP
#define SOL_IS_INTEGER_HPP

#include <sol/object.hpp>

namespace sol::utility {

	inline bool is_integer(const sol::object& object) {
		auto pp = stack::push_pop(object);
		return lua_isinteger(object.lua_state(), -1);
	}
	inline bool is_integer(const sol::stack_object& object) {
		return lua_isinteger(object.lua_state(), object.stack_index());
	}

}

#endif
