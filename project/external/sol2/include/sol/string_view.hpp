





#ifndef SOL_STRING_VIEW_HPP
#define SOL_STRING_VIEW_HPP

#include <sol/version.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <functional>

namespace sol {
	template <typename C, typename T = std::char_traits<C>>
	using basic_string_view = std::basic_string_view<C, T>;

	typedef std::string_view string_view;
	typedef std::wstring_view wstring_view;
	typedef std::u16string_view u16string_view;
	typedef std::u32string_view u32string_view;
	typedef std::hash<std::string_view> string_view_hash;
}

#endif
