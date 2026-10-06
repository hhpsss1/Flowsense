





#ifndef SOL_STACK_CHECK_QUALIFIED_GET_HPP
#define SOL_STACK_CHECK_QUALIFIED_GET_HPP

#include <sol/stack_core.hpp>
#include <sol/stack_check_get_unqualified.hpp>
#include <sol/optional.hpp>

namespace sol { namespace stack {

#if SOL_IS_ON(SOL_COMPILER_GCC)
#pragma GCC diagnostic push
#if !SOL_IS_ON(SOL_COMPILER_CLANG)
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#endif

	namespace stack_detail {
		template <typename OptionalType, typename T, typename Handler>
		OptionalType get_optional(lua_State* L, int index, Handler&& handler, record& tracking) {
			using Tu = meta::unqualified_t<T>;

			if constexpr (is_lua_reference_v<T>) {
				if constexpr (is_global_table_v<Tu>) {
					(void)L;
					(void)index;
					(void)handler;
					tracking.use(1);
					return true;
				}
				else {
					bool success = lua_isnoneornil(L, index) == 0 && stack::check<T>(L, index, &no_panic);
					if (!success) {
						tracking.use(static_cast<int>(success));
						handler(L, index, type::poly, type_of(L, index), "");
						return {};
					}
					return OptionalType(stack_detail::unchecked_get<T>(L, index, tracking));
				}
			}
			else if constexpr (!std::is_reference_v<T> && is_unique_usertype_v<Tu> && !is_actual_type_rebindable_for_v<Tu>) {
				using element = unique_usertype_element_t<Tu>;
				using actual = unique_usertype_actual_t<Tu>;
				tracking.use(1);
				void* memory = lua_touserdata(L, index);
				memory = detail::align_usertype_unique_destructor(memory);
				detail::unique_destructor& pdx = *static_cast<detail::unique_destructor*>(memory);
				if (&detail::usertype_unique_alloc_destroy<element, Tu> == pdx) {
					memory = detail::align_usertype_unique_tag<true, false>(memory);
					memory = detail::align_usertype_unique<actual, true, false>(memory);
					actual* mem = static_cast<actual*>(memory);
					return static_cast<actual>(*mem);
				}
				if constexpr (!derive<element>::value) {
					return OptionalType();
				}
				else {
					memory = detail::align_usertype_unique_tag<true, false>(memory);
					detail::unique_tag& ic = *reinterpret_cast<detail::unique_tag*>(memory);
					memory = detail::align_usertype_unique<actual, true, false>(memory);
					string_view ti = usertype_traits<element>::qualified_name();
					int cast_operation;
					actual r {};
					if constexpr (is_actual_type_rebindable_for_v<Tu>) {
						using rebound_actual_type = unique_usertype_rebind_actual_t<Tu, void>;
						string_view rebind_ti = usertype_traits<rebound_actual_type>::qualified_name();
						cast_operation = ic(memory, &r, ti, rebind_ti);
					}
					else {
						string_view rebind_ti("");
						cast_operation = ic(memory, &r, ti, rebind_ti);
					}
					switch (cast_operation) {
					case 1: {
						actual* mem = static_cast<actual*>(memory);
						return OptionalType(*mem);
					}
					case 2:
						return OptionalType(std::move(r));
					default:
						break;
					}
					return OptionalType();
				}
			}
			else {
				if (!check<T>(L, index, std::forward<Handler>(handler))) {
					tracking.use(static_cast<int>(!lua_isnone(L, index)));
					return OptionalType();
				}
				return OptionalType(stack_detail::unchecked_get<T>(L, index, tracking));
			}
		}
	}

#if SOL_IS_ON(SOL_COMPILER_GCC)
#pragma GCC diagnostic pop
#endif

	template <typename T, typename>
	struct qualified_check_getter {
		typedef decltype(stack_detail::unchecked_get<T>(nullptr, -1, std::declval<record&>())) R;

		template <typename Handler>
		optional<R> get(lua_State* L, int index, Handler&& handler, record& tracking) {
			return stack_detail::get_optional<optional<R>, T>(L, index, std::forward<Handler>(handler), tracking);
		}
	};

	template <typename Optional>
	struct qualified_getter<Optional, std::enable_if_t<meta::is_optional_v<Optional>>> {
		static Optional get(lua_State* L, int index, record& tracking) {
			using T = typename meta::unqualified_t<Optional>::value_type;
			return stack_detail::get_optional<Optional, T>(L, index, &no_panic, tracking);
		}
	};

}}

#endif
