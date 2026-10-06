





#ifndef SOL_OPTIONAL_HPP
#define SOL_OPTIONAL_HPP

#include <sol/forward.hpp>
#include <sol/in_place.hpp>
#include <sol/traits.hpp>
#if SOL_IS_ON(SOL_USE_BOOST)
#include <boost/optional.hpp>
#else
#include <sol/optional_implementation.hpp>
#endif


#include <optional>

namespace sol {

#if SOL_IS_ON(SOL_USE_BOOST)
	template <typename T>
	using optional = boost::optional<T>;
	using nullopt_t = boost::none_t;
	SOL_BOOST_NONE_CONSTEXPR_I_ nullopt_t nullopt = boost::none;
#endif

	namespace meta {
		template <typename T>
		using is_optional = any<is_specialization_of<T, optional>, is_specialization_of<T, std::optional>>;

		template <typename T>
		constexpr inline bool is_optional_v = is_optional<T>::value;
	}

	namespace detail {
		template <typename T>
		struct associated_nullopt {
			inline static constexpr std::nullopt_t value = std::nullopt;
		};

#if SOL_IS_ON(SOL_USE_BOOST)
		template <typename T>
		struct associated_nullopt<boost::optional<T>> {
			inline static SOL_BOOST_NONE_CONSTEXPR_I_ boost::none_t value = boost::none;
		};
#endif

#if SOL_IS_ON(SOL_USE_BOOST)
		template <typename T>
		inline SOL_BOOST_NONE_CONSTEXPR_I_ auto associated_nullopt_v = associated_nullopt<T>::value;
#else
		template <typename T>
		inline constexpr auto associated_nullopt_v = associated_nullopt<T>::value;
#endif
	}
}

#if SOL_IS_ON(SOL_USE_BOOST)
#undef SOL_BOOST_NONE_CONSTEXPR_I_
#endif

#endif
