





#ifndef SOL_STACK_CHECK_QUALIFIED_HPP
#define SOL_STACK_CHECK_QUALIFIED_HPP

#include <sol/stack_check_unqualified.hpp>

namespace sol { namespace stack {

	template <typename X, type expected, typename>
	struct qualified_checker {
		template <typename Handler>
		static bool check(lua_State* L, int index, Handler&& handler, record& tracking) {
			using no_cv_X = meta::unqualified_t<X>;
			if constexpr (!std::is_reference_v<X> && is_unique_usertype_v<no_cv_X>) {
				using element = unique_usertype_element_t<no_cv_X>;
				if constexpr (is_actual_type_rebindable_for_v<no_cv_X>) {
					using rebound_actual_type = unique_usertype_rebind_actual_t<no_cv_X>;
					const type indextype = type_of(L, index);
					tracking.use(1);
					if (indextype != type::userdata) {
						handler(L, index, type::userdata, indextype, "value is not a userdata");
						return false;
					}
					void* memory = lua_touserdata(L, index);
					memory = detail::align_usertype_unique_destructor(memory);
					detail::unique_destructor& pdx = *static_cast<detail::unique_destructor*>(memory);
					if (&detail::usertype_unique_alloc_destroy<element, no_cv_X> == pdx) {
						return true;
					}
					if constexpr (derive<element>::value) {
						memory = detail::align_usertype_unique_tag<true, false>(memory);
						detail::unique_tag& ic = *reinterpret_cast<detail::unique_tag*>(memory);
						string_view ti = usertype_traits<element>::qualified_name();
						string_view rebind_ti = usertype_traits<rebound_actual_type>::qualified_name();
						if (ic(nullptr, nullptr, ti, rebind_ti) != 0) {
							return true;
						}
					}
					handler(L, index, type::userdata, indextype, "value is a userdata but is not the correct unique usertype");
					return false;
				}
				else {
					return stack::unqualified_check<X>(L, index, std::forward<Handler>(handler), tracking);
				}
			}
			else if constexpr (!std::is_reference_v<X> && is_container_v<no_cv_X>) {
				if (type_of(L, index) == type::userdata) {
					return stack::unqualified_check<X>(L, index, std::forward<Handler>(handler), tracking);
				}
				else {
					return stack::unqualified_check<nested<X>>(L, index, std::forward<Handler>(handler), tracking);
				}
			}
			else if constexpr (!std::is_reference_v<X> && meta::is_specialization_of_v<X, nested>) {
				using NestedX = typename meta::unqualified_t<X>::nested_type;
				return stack::check<NestedX>(L, index, ::std::forward<Handler>(handler), tracking);
			}
			else {
				return stack::unqualified_check<X>(L, index, std::forward<Handler>(handler), tracking);
			}
		}
	};
}}

#endif
