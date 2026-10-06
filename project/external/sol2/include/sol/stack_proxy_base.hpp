





#ifndef SOL_STACK_PROXY_BASE_HPP
#define SOL_STACK_PROXY_BASE_HPP

#include <sol/stack.hpp>
#include <sol/proxy_base.hpp>

namespace sol {
	struct stack_proxy_base : public proxy_base<stack_proxy_base> {
	private:
		lua_State* m_L;
		int m_index;

	public:
		stack_proxy_base() : m_L(nullptr), m_index(0) {
		}
		stack_proxy_base(lua_State* L_, int index_) : m_L(L_), m_index(index_) {
		}

		template <typename T>
		decltype(auto) get() const {
			return stack::get<T>(m_L, stack_index());
		}

		template <typename T>
		bool is() const {
			return stack::check<T>(m_L, stack_index());
		}

		template <typename T>
		decltype(auto) as() const {
			return get<T>();
		}

		type get_type() const noexcept {
			return type_of(lua_state(), stack_index());
		}

		int push() const {
			return push(m_L);
		}

		int push(lua_State* L_) const {
			lua_pushvalue(L_, m_index);
			return 1;
		}

		lua_State* lua_state() const {
			return m_L;
		}
		int stack_index() const {
			return m_index;
		}
	};

	namespace stack {
		template <>
		struct unqualified_getter<stack_proxy_base> {
			static stack_proxy_base get(lua_State* L_, int index_ = -1) {
				return stack_proxy_base(L_, index_);
			}
		};

		template <>
		struct unqualified_pusher<stack_proxy_base> {
			static int push(lua_State*, const stack_proxy_base& proxy_reference) {
				return proxy_reference.push();
			}
		};
	}

}

#endif
