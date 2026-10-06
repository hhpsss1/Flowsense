





#ifndef SOL_ERROR_HPP
#define SOL_ERROR_HPP

#include <sol/compatibility.hpp>

#include <stdexcept>
#include <string>
#include <array>

namespace sol {
	namespace detail {
		struct direct_error_tag { };
		const auto direct_error = direct_error_tag {};

		struct error_result {
			int results;
			const char* format_string;
			std::array<const char*, 4> argument_strings;

			error_result() : results(0), format_string(nullptr) {
			}

			error_result(int results_) : results(results_), format_string(nullptr) {
			}

			error_result(const char* format_string_, const char* first_message_) : results(0), format_string(format_string_), argument_strings() {
				argument_strings[0] = first_message_;
			}
		};

		inline int handle_errors(lua_State* L, const error_result& er) {
			if (er.format_string == nullptr) {
				return er.results;
			}
			return luaL_error(L, er.format_string, er.argument_strings[0], er.argument_strings[1], er.argument_strings[2], er.argument_strings[3]);
		}

		class error_exception : public std::runtime_error {
		public:
			error_exception(const std::string& str) : error_exception(detail::direct_error, "lua: error: " + str) {
			}
			error_exception(std::string&& str) : error_exception(detail::direct_error, "lua: error: " + std::move(str)) {
			}
			error_exception(detail::direct_error_tag, const std::string& str) : std::runtime_error(str) {
			}
			error_exception(detail::direct_error_tag, std::string&& str) : std::runtime_error(str) {
			}

			error_exception(const error_exception& e) = default;
			error_exception(error_exception&& e) = default;
			error_exception& operator=(const error_exception& e) = default;
			error_exception& operator=(error_exception&& e) = default;
		};
	}

	class error : public std::runtime_error {
	private:
		std::string what_reason;

	public:
		error(const std::string& str) : error(detail::direct_error, "lua: error: " + str) {
		}
		error(std::string&& str) : error(detail::direct_error, "lua: error: " + std::move(str)) {
		}
		error(detail::direct_error_tag, const std::string& str) : std::runtime_error(str) {
		}
		error(detail::direct_error_tag, std::string&& str) : std::runtime_error(str) {
		}

		error(const error& e) = default;
		error(error&& e) = default;
		error& operator=(const error& e) = default;
		error& operator=(error&& e) = default;
	};

}

#endif
