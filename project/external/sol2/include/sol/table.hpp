





#ifndef SOL_TABLE_HPP
#define SOL_TABLE_HPP

#include <sol/table_core.hpp>
#include <sol/lua_table.hpp>
#include <sol/usertype.hpp>

namespace sol {
	typedef table_core<false> table;

	template <bool is_global, typename base_type>
	template <typename Class, typename Key>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key) {
		constant_automagic_enrollments<> enrollments {};
		return this->new_usertype<Class>(std::forward<Key>(key), std::move(enrollments));
	}

	template <bool is_global, typename base_type>
	template <typename Class, typename Key, automagic_flags enrollment_flags>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key, constant_automagic_enrollments<enrollment_flags> enrollments) {
		int mt_index = u_detail::register_usertype<Class, enrollment_flags>(this->lua_state(), std::move(enrollments));
		usertype<Class> mt(this->lua_state(), -mt_index);
		lua_pop(this->lua_state(), 1);
		set(std::forward<Key>(key), mt);
		return mt;
	}

	template <bool is_global, typename base_type>
	template <typename Class, typename Key>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key, automagic_enrollments enrollments) {
		int mt_index = u_detail::register_usertype<Class, automagic_flags::all>(this->lua_state(), std::move(enrollments));
		usertype<Class> mt(this->lua_state(), -mt_index);
		lua_pop(this->lua_state(), 1);
		set(std::forward<Key>(key), mt);
		return mt;
	}

	template <bool is_global, typename base_type>
	template <typename Class, typename Key, typename Arg, typename... Args, typename>
	usertype<Class> basic_table_core<is_global, base_type>::new_usertype(Key&& key, Arg&& arg, Args&&... args) {
		constexpr automagic_flags enrollment_flags = meta::any_same_v<no_construction, meta::unqualified_t<Arg>, meta::unqualified_t<Args>...>
		     ? clear_flags(automagic_flags::all, automagic_flags::default_constructor)
		     : automagic_flags::all;
		constant_automagic_enrollments<enrollment_flags> enrollments;
		enrollments.default_constructor = !detail::any_is_constructor_v<Arg, Args...>;
		enrollments.destructor = !detail::any_is_destructor_v<Arg, Args...>;
		usertype<Class> ut = this->new_usertype<Class>(std::forward<Key>(key), std::move(enrollments));
		static_assert(sizeof...(Args) % 2 == static_cast<std::size_t>(!detail::any_is_constructor_v<Arg>),
		     "you must pass an even number of arguments to new_usertype after first passing a constructor");
		if constexpr (detail::any_is_constructor_v<Arg>) {
			ut.set(meta_function::construct, std::forward<Arg>(arg));
			ut.tuple_set(std::make_index_sequence<(sizeof...(Args)) / 2>(), std::forward_as_tuple(std::forward<Args>(args)...));
		}
		else {
			ut.tuple_set(std::make_index_sequence<(sizeof...(Args) + 1) / 2>(), std::forward_as_tuple(std::forward<Arg>(arg), std::forward<Args>(args)...));
		}
		return ut;
	}

	template <typename base_type>
	template <typename Key, typename Value>
	basic_metatable<base_type>& basic_metatable<base_type>::set(Key&& key, Value&& value) {
		this->push();
		lua_State* L = this->lua_state();
		int target = lua_gettop(L);
		optional<u_detail::usertype_storage_base&> maybe_uts = nullopt;
		maybe_uts = u_detail::maybe_get_usertype_storage_base(L, target);
		if (maybe_uts) {
			u_detail::usertype_storage_base& uts = *maybe_uts;
			uts.set(L, std::forward<Key>(key), std::forward<Value>(value));
			return *this;
		}
		else {
			base_t::set(std::forward<Key>(key), std::forward<Value>(value));
		}
		this->pop();
		return *this;
	}

	namespace stack {
		template <>
		struct unqualified_getter<metatable_key_t> {
			static metatable get(lua_State* L, int index = -1) {
				if (lua_getmetatable(L, index) == 0) {
					return metatable(L, ref_index(LUA_REFNIL));
				}
				return metatable(L, -1);
			}
		};
	}
}

#endif
