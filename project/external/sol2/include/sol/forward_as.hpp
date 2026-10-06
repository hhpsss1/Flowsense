





#ifndef SOL_FORWARD_AS_HPP
#define SOL_FORWARD_AS_HPP

#include <sol/version.hpp>

#include <utility>
#include <type_traits>

namespace sol {
	template <typename T, typename U>
	constexpr decltype(auto) forward_as(U&& value) noexcept {
		if constexpr (::std::is_lvalue_reference_v<T>) {
			return value;
		}
		else {
			return ::std::move(value);
		}
	}
}

#endif
