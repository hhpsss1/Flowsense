





#ifndef SOL_IN_PLACE_HPP
#define SOL_IN_PLACE_HPP

#include <cstddef>
#include <utility>

namespace sol {

	using in_place_t = std::in_place_t;
	constexpr std::in_place_t in_place {};
	constexpr std::in_place_t in_place_of {};

	template <typename T>
	using in_place_type_t = std::in_place_type_t<T>;
	template <typename T>
	constexpr std::in_place_type_t<T> in_place_type {};

	template <size_t I>
	using in_place_index_t = std::in_place_index_t<I>;
	template <size_t I>
	constexpr in_place_index_t<I> in_place_index {};

}

#endif
