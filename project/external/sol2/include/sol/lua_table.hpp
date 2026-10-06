





#ifndef SOL_LUA_TABLE_HPP
#define SOL_LUA_TABLE_HPP

#include <sol/table_core.hpp>

namespace sol {

	template <typename ref_t>
	struct basic_lua_table : basic_table_core<false, ref_t> {
	private:
		using base_t = basic_table_core<false, ref_t>;

		friend class state;
		friend class state_view;

	public:
		using base_t::lua_state;

		basic_lua_table() noexcept = default;
		basic_lua_table(const basic_lua_table&) = default;
		basic_lua_table(basic_lua_table&&) = default;
		basic_lua_table& operator=(const basic_lua_table&) = default;
		basic_lua_table& operator=(basic_lua_table&&) = default;
		basic_lua_table(const stack_reference& r) : basic_lua_table(r.lua_state(), r.stack_index()) {
		}
		basic_lua_table(stack_reference&& r) : basic_lua_table(r.lua_state(), r.stack_index()) {
		}
		template <typename T, meta::enable_any<is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_lua_table(lua_State* L, T&& r) : base_t(L, std::forward<T>(r)) {
#if SOL_IS_ON(SOL_SAFE_REFERENCES)
			auto pp = stack::push_pop(*this);
			constructor_handler handler {};
			stack::check<basic_lua_table>(lua_state(), -1, handler);
#endif
		}
		basic_lua_table(lua_State* L, const new_table& nt) : base_t(L, nt) {
			if (!is_stack_based<meta::unqualified_t<ref_t>>::value) {
				lua_pop(L, 1);
			}
		}
		basic_lua_table(lua_State* L, int index = -1) : base_t(detail::no_safety, L, index) {
#if SOL_IS_ON(SOL_SAFE_REFERENCES)
			constructor_handler handler {};
			stack::check<basic_lua_table>(L, index, handler);
#endif
		}
		basic_lua_table(lua_State* L, ref_index index) : base_t(detail::no_safety, L, index) {
#if SOL_IS_ON(SOL_SAFE_REFERENCES)
			auto pp = stack::push_pop(*this);
			constructor_handler handler {};
			stack::check<basic_lua_table>(lua_state(), -1, handler);
#endif
		}
		template <typename T,
		     meta::enable<meta::neg<meta::any_same<meta::unqualified_t<T>, basic_lua_table>>, meta::neg<std::is_same<ref_t, stack_reference>>,
		          meta::neg<std::is_same<lua_nil_t, meta::unqualified_t<T>>>, is_lua_reference<meta::unqualified_t<T>>> = meta::enabler>
		basic_lua_table(T&& r) noexcept : basic_lua_table(detail::no_safety, std::forward<T>(r)) {
#if SOL_IS_ON(SOL_SAFE_REFERENCES)
			if (!is_table<meta::unqualified_t<T>>::value) {
				auto pp = stack::push_pop(*this);
				constructor_handler handler {};
				stack::check<basic_lua_table>(lua_state(), -1, handler);
			}
#endif
		}
		basic_lua_table(lua_nil_t r) noexcept : basic_lua_table(detail::no_safety, r) {
		}
	};

}

#endif
