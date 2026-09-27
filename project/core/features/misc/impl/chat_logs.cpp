#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/settings.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>

namespace features::misc {

	namespace chat_detail {

		typedef __int64(__fastcall* getLolqword_23D0A50_t)();
		inline getLolqword_23D0A50_t getLolqword_23D0A50_o;

		typedef __int64(__fastcall* printToChat_t)(__int64 chat_ptr, unsigned int color_or_id, const char* text, ...);
		inline printToChat_t printToChat_o;

		typedef __int64(__fastcall* send_message_client_t)(void* hud_voice, const char* text, unsigned int color_or_id, std::uint8_t* flags);

	} // namespace chat_detail

	std::string chat_logs::controller_name( std::uintptr_t controller )
	{
		if ( !controller )
		{
			return {};
		}

		const auto name_ptr = memory::safe_read<std::uintptr_t>(
			controller + SCHEMA( "CCSPlayerController", "m_sSanitizedPlayerName"_hash ) ).value_or( 0 );

		return memory::read_string( name_ptr, 64 );
	}

	std::string chat_logs::lookup_name( int userid, std::uintptr_t controller ) const
	{
		if ( userid > 0 )
		{
			const auto it = this->m_name_cache.find( userid );
			if ( it != this->m_name_cache.end( ) && !it->second.empty( ) )
			{
				return it->second;
			}
		}

		return this->controller_name( controller );
	}

	void chat_logs::print( const std::string& message )
	{
		if ( message.empty( ) )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || !local.pawn || systems::g_local.is_in_cinematic( ) )
		{
			return;
		}

		static auto print_to_chat_fn = PATTERN(patterns::print_to_chat);
		static auto get_chat_qword_fn = PATTERN(patterns::get_chat_qword);

		if ( !print_to_chat_fn || !get_chat_qword_fn )
		{
			return;
		}

		__try
		{
			if ( !chat_detail::printToChat_o )
			{
				chat_detail::printToChat_o = reinterpret_cast<chat_detail::printToChat_t>(print_to_chat_fn);
			}

			if ( !chat_detail::getLolqword_23D0A50_o )
			{
				chat_detail::getLolqword_23D0A50_o = reinterpret_cast<chat_detail::getLolqword_23D0A50_t>(get_chat_qword_fn);
			}

			const auto chat_ptr = chat_detail::getLolqword_23D0A50_o();
			if ( !chat_ptr )
			{
				return;
			}

			chat_detail::printToChat_o(chat_ptr, 0xFFFFFFFFu, message.c_str());
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	void chat_logs::on_vote_cast( std::uintptr_t event )
	{
		const auto& cfg = settings::g_misc.m_chat_logs;
		if ( !event || !cfg.enabled.value || !cfg.votekick.value )
		{
			return;
		}

		const auto local = systems::g_local.get( );
		if ( !local.is_valid( ) || !local.controller )
		{
			return;
		}

		const auto userid_key = cstypes::event_hash{ 0, "userid" };
		const auto userid = memory::call<int>( PATTERN( patterns::game_event_get_int ), event, "userid", false );
		const auto voter = memory::call<std::uintptr_t>( PATTERN( patterns::game_event_get_controller ), event, &userid_key );
		const auto voter_name = this->lookup_name( userid, voter );
		if ( voter_name.empty( ) )
		{
			return;
		}

		const auto vote_option = memory::call<int>( PATTERN( patterns::game_event_get_int ), event, "vote_option", 0 );

		std::string msg = "\x0A\x02[nexoria.cx] \x01" + voter_name + " voted " + ( vote_option == 0 ? "\x04YES\x01" : "\x02NO\x01" );

		const auto entityid_key = cstypes::event_hash{ 0, "entityid" };
		const auto target = memory::call<std::uintptr_t>( PATTERN( patterns::game_event_get_controller ), event, &entityid_key );

		if ( target == local.controller )
		{
			msg += " on \x03you\x01";
		}
		else
		{
			const auto target_name = this->controller_name( target );
			if ( !target_name.empty( ) )
			{
				msg += " on \x03" + target_name + "\x01";
			}
		}

		this->print( msg );
	}

	void chat_logs::on_player_info( std::uintptr_t event )
	{
		if ( !event )
		{
			return;
		}

		const auto userid = memory::call<int>( PATTERN( patterns::game_event_get_int ), event, "userid", false );
		if ( userid <= 0 )
		{
			return;
		}

		const auto userid_key = cstypes::event_hash{ 0, "userid" };
		const auto controller = memory::call<std::uintptr_t>( PATTERN( patterns::game_event_get_controller ), event, &userid_key );

		const auto name = this->controller_name( controller );
		if ( name.empty( ) )
		{
			return;
		}

		this->m_name_cache[ userid ] = name;
	}

	void chat_logs::print_hit( const std::string& name, int damage, const std::string& hitgroup, int health, const std::string& reason, std::uint32_t weapon_type )
	{
		std::string msg = "\x0A\x02[nexoria.cx] \x01";

		if ( weapon_type == cstypes::weapon_type::taser )
		{
			msg += "zapped \x06the fuck out of\x01 \x03" + name + "\x01";
		}
		else if ( weapon_type == cstypes::weapon_type::knife )
		{
			msg += "knifed \x03" + name + "\x01 for \x06" + std::to_string( damage ) + "\x01 (" + std::to_string( health ) + " remaining)";
		}
		else
		{
			msg += "Hit \x06" + std::to_string( damage ) + "\x01 to \x03" + name + "\x01 in \x06" + hitgroup + "\x01 (" + std::to_string( health ) + " remaining)";
			if ( !reason.empty( ) )
			{
				msg += ", \x05" + reason + "\x01";
			}
		}

		this->print( msg );
	}

	void chat_logs::print_miss( const std::string& name, const std::string& group, const char* reason, float hitchance, float damage, bool forced, std::uint32_t weapon_type )
	{
		std::string msg = "\x0A\x02[nexoria.cx] \x01";

		if ( weapon_type == cstypes::weapon_type::knife )
		{
			msg += "missed knife on \x03" + name + "\x01 due to \x05latency\x01";
		}
		else if ( weapon_type == cstypes::weapon_type::taser )
		{
			msg += "missed zeus on \x03" + name + "\x01";
		}
		else if ( forced )
		{
			msg += "missed \x03" + name + "\x01 (forced shot, \x05" + std::to_string( static_cast< int >( std::round( hitchance * 100.0f ) ) ) + "% hitchance\x01)";
		}
		else
		{
			const auto hc = std::to_string( static_cast< int >( std::round( hitchance * 100.0f ) ) );
			const auto dmg = std::to_string( static_cast< int >( std::round( damage ) ) );
			msg += "missed \x03" + name + "\x01, targeted \x06" + group + "\x01 (hc=\x05" + hc + "%\x01, dmg=\x05" + dmg + "\x01, reason=\x05" + ( reason ? reason : "" ) + "\x01)";
		}

		this->print( msg );
	}

} // namespace features::misc