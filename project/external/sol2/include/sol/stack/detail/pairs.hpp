





#ifndef SOL_DETAIL_PAIRS_HPP
#define SOL_DETAIL_PAIRS_HPP

#include <sol/version.hpp>

#include <sol/stack.hpp>
#include <sol/stack_reference.hpp>
#include <sol/protected_function.hpp>
#include <sol/assert.hpp>

#include <optional>

namespace sol { namespace stack { namespace stack_detail {

	inline bool maybe_push_lua_next_function(lua_State* L_) {
		stack::get_field<true, false>(L_, "next");
		bool is_next = stack::check<protected_function>(L_);
		if (is_next) {
			return true;
		}
		stack::get_field<true, false>(L_, "table");
		stack::record tracking{};
		if (!stack::loose_table_check(L_, -1, &no_panic, tracking)) {
			return false;
		}
		lua_getfield(L_, -1, "next");
		bool is_table_next_func = stack::check<protected_function>(L_, -1);
		if (is_table_next_func) {
			return true;
		}
		lua_pop(L_, 1);
		return false;
	}

	inline std::optional<protected_function> find_lua_next_function(lua_State* L_) {
		if (maybe_push_lua_next_function(L_)) {
			return stack::pop<protected_function>(L_);
		}
		return std::nullopt;
	}

	inline int c_lua_next(lua_State* L_) noexcept {
		stack_reference table_stack_ref(L_, raw_index(1));
		stateless_stack_reference key_stack_ref(L_, raw_index(2));
		int result = lua_next(table_stack_ref.lua_state(), table_stack_ref.stack_index());
		if (result == 0) {
			stack::push(L_, lua_nil);
			return 1;
		}
		return 2;
	}

	inline int readonly_pairs(lua_State* L_) noexcept {
		int pushed = 0;
		if (!maybe_push_lua_next_function(L_)) {
			pushed += stack::push(L_, &c_lua_next);
		}
		else {
			pushed += 1;
		}
		int metatable_exists = lua_getmetatable(L_, 1);
		SOL_ASSERT(metatable_exists == 1);
		const auto& index_key = to_string(sol::meta_function::index);
		lua_getfield(L_, lua_gettop(L_), index_key.c_str());
		lua_remove(L_, -2);
		pushed += 1;
		pushed += stack::push(L_, lua_nil);
		return pushed;
	}

}}}

#endif
