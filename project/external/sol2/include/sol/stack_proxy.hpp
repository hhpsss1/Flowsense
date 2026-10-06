





#ifndef SOL_STACK_PROXY_HPP
#define SOL_STACK_PROXY_HPP

#include <sol/stack_proxy_base.hpp>

namespace sol {
	struct stack_proxy : public stack_proxy_base {
	public:
		stack_proxy() : stack_proxy_base() {
		}
		stack_proxy(lua_State* L, int index) : stack_proxy_base(L, index) {
		}

		template <typename... Ret, typename... Args>
		decltype(auto) call(Args&&... args);

		template <typename... Args>
		decltype(auto) operator()(Args&&... args) {
			return call<>(std::forward<Args>(args)...);
		}
	};

	namespace stack {
		template <>
		struct unqualified_getter<stack_proxy> {
			static stack_proxy get(lua_State* L, int index, record& tracking) {
				tracking.use(0);
				return stack_proxy(L, index);
			}
		};

		template <>
		struct unqualified_pusher<stack_proxy> {
			static int push(lua_State*, const stack_proxy& ref) {
				return ref.push();
			}
		};
	}
}

#endif
