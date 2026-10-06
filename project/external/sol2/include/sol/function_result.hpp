





#ifndef SOL_FUNCTION_RESULT_HPP
#define SOL_FUNCTION_RESULT_HPP

#include <sol/protected_function_result.hpp>
#include <sol/unsafe_function_result.hpp>

#include <cstdint>

namespace sol {

	namespace detail {
		template <>
		struct is_speshul<unsafe_function_result> : std::true_type { };
		template <>
		struct is_speshul<protected_function_result> : std::true_type { };

		template <std::size_t I, typename... Args, typename T>
		stack_proxy get(types<Args...>, meta::index_value<0>, meta::index_value<I>, const T& fr) {
			return stack_proxy(fr.lua_state(), fr.stack_index() + static_cast<int>(I));
		}

		template <std::size_t I, std::size_t N, typename Arg, typename... Args, typename T, meta::enable<meta::boolean<(N > 0)>> = meta::enabler>
		stack_proxy get(types<Arg, Args...>, meta::index_value<N>, meta::index_value<I>, const T& fr) {
			return get(types<Args...>(), meta::index_value<N - 1>(), meta::index_value<I + lua_size<Arg>::value>(), fr);
		}
	}

	template <>
	struct tie_size<unsafe_function_result> : std::integral_constant<std::size_t, SIZE_MAX> { };

	template <>
	struct tie_size<protected_function_result> : std::integral_constant<std::size_t, SIZE_MAX> { };

	template <std::size_t I>
	stack_proxy get(const unsafe_function_result& fr) {
		return stack_proxy(fr.lua_state(), fr.stack_index() + static_cast<int>(I));
	}

	template <std::size_t I, typename... Args>
	stack_proxy get(types<Args...> t, const unsafe_function_result& fr) {
		return detail::get(t, meta::index_value<I>(), meta::index_value<0>(), fr);
	}

	template <std::size_t I>
	stack_proxy get(const protected_function_result& fr) {
		return stack_proxy(fr.lua_state(), fr.stack_index() + static_cast<int>(I));
	}

	template <std::size_t I, typename... Args>
	stack_proxy get(types<Args...> t, const protected_function_result& fr) {
		return detail::get(t, meta::index_value<I>(), meta::index_value<0>(), fr);
	}
}

#endif
