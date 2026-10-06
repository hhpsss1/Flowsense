





#ifndef SOL_FUNCTION_TYPES_OVERLOAD_HPP
#define SOL_FUNCTION_TYPES_OVERLOAD_HPP

#include <sol/overload.hpp>
#include <sol/call.hpp>
#include <sol/function_types_core.hpp>

namespace sol { namespace function_detail {
	template <int start_skew, typename... Functions>
	struct overloaded_function {
		typedef std::tuple<Functions...> overload_list;
		typedef std::make_index_sequence<sizeof...(Functions)> indices;
		overload_list overloads;

		overloaded_function(overload_list set) : overloads(std::move(set)) {
		}

		overloaded_function(Functions... fxs) : overloads(fxs...) {
		}

		template <typename Fx, std::size_t I, typename... R, typename... Args>
		static int call(types<Fx>, meta::index_value<I>, types<R...>, types<Args...>, lua_State* L, int, int, overload_list& ol) {
			auto& func = std::get<I>(ol);
			int nr = call_detail::call_wrapped<void, true, false, start_skew>(L, func);
			return nr;
		}

		struct on_success {
			template <typename... Args>
			int operator()(Args&&... args) const {
				return call(std::forward<Args>(args)...);
			}
		};

		int operator()(lua_State* L) {
			on_success call_obj {};
			return call_detail::overload_match<Functions...>(call_obj, L, 1 + start_skew, overloads);
		}
	};
}}

#endif
