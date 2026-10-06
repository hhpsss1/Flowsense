





#ifndef SOL_AS_RETURNS_HPP
#define SOL_AS_RETURNS_HPP

#include <sol/traits.hpp>
#include <sol/stack.hpp>
#include <sol/ebco.hpp>

namespace sol {
	template <typename T>
	struct as_returns_t : private detail::ebco<T> {
	private:
		using base_t = detail::ebco<T>;

	public:
		using base_t::base_t;
		using base_t::value;
	};

	template <typename Source>
	auto as_returns(Source&& source) {
		return as_returns_t<std::decay_t<Source>> { std::forward<Source>(source) };
	}

	namespace stack {
		template <typename T>
		struct unqualified_pusher<as_returns_t<T>> {
			int push(lua_State* L, const as_returns_t<T>& e) {
				auto& src = detail::unwrap(e.value());
				int p = 0;
				for (const auto& i : src) {
					p += stack::push(L, i);
				}
				return p;
			}
		};
	}
}

#endif
