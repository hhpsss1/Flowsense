





#ifndef SOL_FUNCTION_HPP
#define SOL_FUNCTION_HPP

#include <sol/stack.hpp>
#include <sol/unsafe_function.hpp>
#include <sol/protected_function.hpp>
#include <sol/bytecode.hpp>
#include <functional>

namespace sol {
	template <typename... Ret, typename... Args>
	decltype(auto) stack_proxy::call(Args&&... args) {
		stack_function sf(this->lua_state(), this->stack_index());
		return sf.template call<Ret...>(std::forward<Args>(args)...);
	}

	inline protected_function_result::protected_function_result(unsafe_function_result&& o) noexcept
	: L(o.lua_state()), index(o.stack_index()), returncount(o.return_count()), popcount(o.return_count()), err(o.status()) {
		o.abandon();
	}

	inline protected_function_result& protected_function_result::operator=(unsafe_function_result&& o) noexcept {
		L = o.lua_state();
		index = o.stack_index();
		returncount = o.return_count();
		popcount = o.return_count();
		err = o.status();
		o.abandon();
		return *this;
	}

	inline unsafe_function_result::unsafe_function_result(protected_function_result&& o) noexcept
	: L(o.lua_state()), index(o.stack_index()), returncount(o.return_count()) {
		o.abandon();
	}
	inline unsafe_function_result& unsafe_function_result::operator=(protected_function_result&& o) noexcept {
		L = o.lua_state();
		index = o.stack_index();
		returncount = o.return_count();
		o.abandon();
		return *this;
	}

	namespace detail {
		template <typename... R>
		struct std_shim {
			unsafe_function lua_func_;

			std_shim(unsafe_function lua_func) : lua_func_(std::move(lua_func)) {
			}

			template <typename... Args>
			meta::return_type_t<R...> operator()(Args&&... args) {
				return lua_func_.call<R...>(std::forward<Args>(args)...);
			}
		};

		template <>
		struct std_shim<void> {
			unsafe_function lua_func_;

			std_shim(unsafe_function lua_func) : lua_func_(std::move(lua_func)) {
			}

			template <typename... Args>
			void operator()(Args&&... args) {
				lua_func_.call<void>(std::forward<Args>(args)...);
			}
		};
	}

	namespace stack {
		template <typename Signature>
		struct unqualified_getter<std::function<Signature>> {
			typedef meta::bind_traits<Signature> fx_t;
			typedef typename fx_t::args_list args_lists;
			typedef meta::tuple_types<typename fx_t::return_type> return_types;

			template <typename... R>
			static std::function<Signature> get_std_func(types<R...>, lua_State* L, int index) {
				detail::std_shim<R...> fx(unsafe_function(L, index));
				return fx;
			}

			static std::function<Signature> get(lua_State* L, int index, record& tracking) {
				tracking.use(1);
				type t = type_of(L, index);
				if (t == type::none || t == type::lua_nil) {
					return nullptr;
				}
				return get_std_func(return_types(), L, index);
			}
		};

		template <typename Allocator>
		struct unqualified_getter<basic_bytecode<Allocator>> {
			static basic_bytecode<Allocator> get(lua_State* L, int index, record& tracking) {
				tracking.use(1);
				stack_function sf(L, index);
				return sf.dump(&dump_panic_on_error);
			}
		};
	}

}

#endif
