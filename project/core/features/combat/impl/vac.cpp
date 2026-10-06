#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <core/features/features.hpp>
#include <protection/game_addresses.hpp>

#include <cmath>

namespace features::combat {

	namespace
	{
		constexpr int k_input_history_capacity{ 16 };
		constexpr float k_when_base{ 0.023590f };
		constexpr float k_when_delta{ 0.908453f };
	}

	void vac::on_create_move( systems::input::usercmd* cmd )
	{
		if ( !cmd )
		{
			return;
		}

		auto base = cmd->csgo_user_cmd.mutable_base( );
		if ( !base )
		{
			return;
		}

		if ( !g_rage.is_firing_this_tick( ) )
		{
			return;
		}

		auto view_angles = base->mutable_viewangles( );
		if ( !view_angles )
		{
			auto history_field = cmd->csgo_user_cmd.mutable_input_history( );
			if ( !history_field )
			{
				return;
			}

			const auto raw_va = memory::call<void*>(PATTERN (patterns::qangle_alloc), history_field->m_arena );
			if ( !raw_va )
			{
				return;
			}

			base->m_viewangles = reinterpret_cast<proto::msg_qangle*>( raw_va );
			view_angles = base->mutable_viewangles( );
			if ( !view_angles )
			{
				return;
			}
		}

		const math::vector3 prior{ view_angles->x( ), view_angles->y( ), view_angles->z( ) };

		const math::vector3 delta
		{
			this->wrap180( prior.x - this->m_reference.x ),
			this->wrap180( prior.y - this->m_reference.y ),
			this->wrap180( prior.z - this->m_reference.z )
		};

		auto history_field = cmd->csgo_user_cmd.mutable_input_history( );
		if ( !history_field )
		{
			return;
		}

		int cached_tick_count{ 0 };
		for ( int i = 0; i < history_field->m_current_size; i++ )
		{
			const auto entry = cmd->csgo_user_cmd.mutable_input_history( i );
			if ( entry )
			{
				cached_tick_count = entry->render_tick_count( );
			}
		}

		const int remaining = k_input_history_capacity - history_field->m_current_size;
		if ( remaining <= 0 )
		{
			this->m_reference = prior;
			return;
		}

		int emitted{ 0 };
		for ( int iter = 0; iter < remaining; iter++ )
		{
			const float fraction = static_cast< float >( iter + 1 ) / static_cast< float >( remaining );
			const float when = k_when_base + fraction * k_when_delta;

			const math::vector3 interp
			{
				prior.x + fraction * delta.x,
				prior.y + fraction * delta.y,
				prior.z + fraction * delta.z
			};

			systems::input::input_history_params params{};
			params.view_angles = interp;
			params.render_tick = cached_tick_count;
			params.render_frac = when;
			params.player_tick = 0;
			params.player_frac = 0;

			const auto entry = systems::g_input.push_input_history( cmd, params );
			if ( !entry )
			{
				break;
			}

			entry->m_has_bits.clear( 0xFFFFFFFFu );
			entry->m_has_bits.set( 0x1E01u );

			emitted++;
		}

		this->m_reference = prior;

		if ( emitted > 0 )
		{
			const auto seed = memory::call<std::uint32_t>(PATTERN (patterns::get_tick_view_angles), nullptr, &prior, cached_tick_count );
			base->set_random_seed( static_cast< std::int32_t >( seed ) );
		}
	}

	void vac::on_level_init( )
	{
		this->m_reference = {};
	}

	float vac::wrap180( float value ) const
	{
		float result = std::remainderf( value, 360.0f );
		if ( result > 180.0f )
		{
			result -= 360.0f;
		}
		else if ( result < -180.0f )
		{
			result += 360.0f;
		}

		return result;
	}

}