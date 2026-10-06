





#ifndef SOL_LOAD_RESULT_HPP
#define SOL_LOAD_RESULT_HPP

#include <sol/stack.hpp>
#include <sol/function.hpp>
#include <sol/proxy_base.hpp>
#include <cstdint>

namespace sol {
	struct load_result : public proxy_base<load_result> {
	private:
		lua_State* L;
		int index;
		int returncount;
		int popcount;
		load_status err;

	public:
		load_result() noexcept : load_result(nullptr) {}
		load_result(lua_State* Ls, int stackindex = -1, int retnum = 0, int popnum = 0, load_status lerr = load_status::ok) noexcept
		: L(Ls), index(stackindex), returncount(retnum), popcount(popnum), err(lerr) {
		}

		load_result(const load_result&) = delete;
		load_result& operator=(const load_result&) = delete;

		load_result(load_result&& o) noexcept : L(o.L), index(o.index), returncount(o.returncount), popcount(o.popcount), err(o.err) {
			o.L = nullptr;
			o.index = 0;
			o.returncount = 0;
			o.popcount = 0;
			o.err = load_status::syntax;
		}
		load_result& operator=(load_result&& o) noexcept {
			L = o.L;
			index = o.index;
			returncount = o.returncount;
			popcount = o.popcount;
			err = o.err;
			o.L = nullptr;
			o.index = 0;
			o.returncount = 0;
			o.popcount = 0;
			o.err = load_status::syntax;
			return *this;
		}

		load_status status() const noexcept {
			return err;
		}

		bool valid() const noexcept {
			return status() == load_status::ok;
		}

		template <typename T>
		T get() const {
			using UT = meta::unqualified_t<T>;
			if constexpr (meta::is_optional_v<UT>) {
				using ValueType = typename UT::value_type;
				if constexpr (std::is_same_v<ValueType, error>) {
					if (valid()) {
						return UT(nullopt);
					}
					return stack::stack_detail::get_error(L, index);
				}
				else {
					if (!valid()) {
						return UT(nullopt);
					}
					return stack::get<UT>(L, index);
				}
			}
			else {
				if constexpr (std::is_same_v<T, error>) {
#if SOL_IS_ON(SOL_SAFE_PROXIES)
					if (valid()) {
						type_panic_c_str(L, index, type_of(L, index), type::none, "expecting an error type (a string, from Lua)");
					}
#endif
					return stack::stack_detail::get_error(L, index);
				}
				else {
#if SOL_IS_ON(SOL_SAFE_PROXIES)
					if (!valid()) {
						type_panic_c_str(L, index, type_of(L, index), type::none);
					}
#endif
					return stack::get<T>(L, index);
				}
			}
		}

		template <typename... Ret, typename... Args>
		decltype(auto) call(Args&&... args) {
			return get<protected_function>().template call<Ret...>(std::forward<Args>(args)...);
		}

		template <typename... Args>
		decltype(auto) operator()(Args&&... args) {
			return call<>(std::forward<Args>(args)...);
		}

		lua_State* lua_state() const noexcept {
			return L;
		};
		int stack_index() const noexcept {
			return index;
		};

		~load_result() {
			if (L != nullptr) {
				stack::remove(L, index, popcount);
			}
		}
	};
}

#endif
