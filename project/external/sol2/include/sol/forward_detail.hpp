





#ifndef SOL_FORWARD_DETAIL_HPP
#define SOL_FORWARD_DETAIL_HPP

#include <sol/version.hpp>

#include <sol/forward.hpp>
#include <sol/traits.hpp>

namespace sol {
	namespace detail {
		const bool default_safe_function_calls =
#if SOL_IS_ON(SOL_SAFE_FUNCTION_CALLS)
		     true;
#else
		     false;
#endif
	}


	namespace meta { namespace meta_detail {
	}}

	namespace stack { namespace stack_detail {
		using undefined_method_func = void (*)(stack_reference);

		template <typename T>
		void set_undefined_methods_on(stack_reference);

		struct undefined_metatable;
	}}
}

#endif
