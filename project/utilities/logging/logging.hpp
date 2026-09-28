#pragma once

namespace logging {

	namespace console {

		bool initialize( );

		void print_raw( const char* text );

		template <typename... args_t>
		void print( std::string_view fmt, args_t&&... args )
		{
			print_raw( std::vformat( fmt, std::make_format_args( args... ) ).c_str( ) );
		}

		inline thread_local bool emitting{};

		// Hitlog-канал: хит/мисс логи всегда проходят, даже когда включена
		// чистая консоль. Всё остальное через print() в clean-режиме дропается.
		inline thread_local bool hitlog_bypass{};
		inline std::atomic_bool clean_console{ false };

		inline void set_clean_console( bool enabled )
		{
			clean_console.store( enabled, std::memory_order_relaxed );
		}

		template <typename... args_t>
		void print_hitlog( std::string_view fmt, args_t&&... args )
		{
			hitlog_bypass = true;
			print( fmt, std::forward<args_t>( args )... );
			hitlog_bypass = false;
		}

	} // namespace console

	namespace popup {

		bool initialize( );
		void show( const char* title, const char* message );

	} // namespace popup

} // namespace logging
