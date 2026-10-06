





#ifndef SOL_TO_STRING_HPP
#define SOL_TO_STRING_HPP

#include <sol/forward.hpp>
#include <sol/object.hpp>

#include <cstddef>
#include <string>

namespace sol::utility {

	inline std::string to_string(const sol::stack_object& object) {
		std::size_t len;
		const char* str = luaL_tolstring(object.lua_state(), object.stack_index(), &len);

		auto result = std::string(str, len);

		lua_pop(object.lua_state(), 1);

		return result;
	}

	inline std::string to_string(const sol::object& object) {
		auto pp = sol::stack::push_pop(object);
		return to_string(sol::stack_object(object.lua_state(), -1));
	}

}

#endif
