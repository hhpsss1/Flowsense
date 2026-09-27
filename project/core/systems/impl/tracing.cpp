#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <protection/game_addresses.hpp>
#include "../systems.hpp"

namespace systems {

	bool tracing::is_visible( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, std::uintptr_t skip_entity, std::uintptr_t mask ) const
	{
		auto current_start = start;
		auto entity_to_skip = skip_entity;

		constexpr auto max_penetrations{ 3 };

		for ( auto i = 0; i < max_penetrations; ++i )
		{
			const auto result = this->trace( current_start, end, entity_to_skip, mask );

			if ( result.hit_entity == target_entity || result.fraction > 0.97f )
			{
				return true;
			}

			if ( !result.hit_entity )
			{
				break;
			}

			const auto hit_health = memory::safe_read<int>( result.hit_entity + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) ).value_or( -1 );
			if ( hit_health > 0 && hit_health <= 100 )
			{
				entity_to_skip = result.hit_entity;
				current_start = result.end_pos + ( end - current_start ).normalized( );
				continue;
			}

			break;
		}

		return false;
	}

	tracing::result tracing::trace( const math::vector3& start, const math::vector3& end, std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		auto filter = this->make_filter( skip_entity, mask, layer );
		return this->trace( start, end, filter );
	}

	tracing::result tracing::trace( const math::vector3& start, const math::vector3& end, const filter& filter ) const
	{
		ray ray{};
		result result{};

		memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, &filter, &result );

		return result;
	}

	tracing::result tracing::trace_hull( const math::vector3& start, const math::vector3& end, const math::vector3& mins, const math::vector3& maxs, std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		const auto filter = this->make_filter( skip_entity, mask, layer );
		return this->trace_hull( start, end, mins, maxs, filter );
	}

	tracing::result tracing::trace_hull( const math::vector3& start, const math::vector3& end, const math::vector3& mins, const math::vector3& maxs, const filter& filter ) const
	{
		ray ray{};
		ray.mins = mins;
		ray.maxs = maxs;
		ray.type = 2;

		result result{};

		memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, &filter, &result );

		return result;
	}

	tracing::result tracing::trace_sphere( const math::vector3& start, const math::vector3& end, float radius, const filter& filter ) const
	{
		ray ray{};
		ray.mins = {};
		*reinterpret_cast< float* >( reinterpret_cast< std::uintptr_t >( &ray ) + 12 ) = radius;
		ray.type = 1;

		result result{};

		memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, &filter, &result );

		return result;
	}

	tracing::result tracing::trace_to_entity( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		const auto filter = this->make_filter( skip_entity, mask, layer );
		return this->trace_to_entity( start, end, target_entity, filter );
	}

	tracing::result tracing::trace_to_entity( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, const filter& filter ) const
	{
		ray ray{};
		result result{};

		memory::call<bool>(PATTERN (patterns::trace_ray_entity), addresses::globals::game_trace_manager, &ray, &start, &end, target_entity, &filter, &result );

		return result;
	}

	tracing::filter tracing::make_filter( std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer, int type ) const
	{
		filter filter{};

		memory::call<void>(PATTERN (patterns::trace_filter_init), &filter, skip_entity, mask, layer, type );

		return filter;
	}

	tracing::filter tracing::make_filter( std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const
	{
		filter filter{};

		memory::call<void>(PATTERN (patterns::trace_filter_init), &filter, skip_entity, mask, layer, 7 );

		return filter;
	}

	tracing::player_movement_filter tracing::make_player_movement_filter( std::uintptr_t entity, std::uintptr_t mask, std::uint8_t collision_group ) const
	{
		player_movement_filter filter{};

		// trace_filter_set_collision resolves in this build but its >E8 call
		// target does not accept this argument layout (crashes inside the engine
		// trace with an invalid handle deref). Seed the movement filter with the
		// proven trace_filter_init path used by the render-thread traces instead.
		static_cast< void >( collision_group );
		memory::call<void>(PATTERN (patterns::trace_filter_init), &filter, entity, mask, 4, 7 );

		return filter;
	}

	tracing::result tracing::trace_player_bbox( const math::vector3& start, const math::vector3& end, const bbox_collision& bbox, const player_movement_filter& filter, std::uintptr_t movement_services ) const
	{
		// The dedicated trace_hull >E8 call target also crashes for this build;
		// reuse the standard TraceShape (trace_ray) hull path with the bbox set
		// on the ray, matching the known-good camera/visibility traces.
		static_cast< void >( movement_services );

		ray ray{};
		ray.mins = bbox.mins;
		ray.maxs = bbox.maxs;
		ray.type = 2;

		result result{};

		memory::call<bool>(PATTERN (patterns::trace_ray), addresses::globals::game_trace_manager, &ray, &start, &end, reinterpret_cast< const struct filter* >( &filter ), &result );

		return result;
	}

	void tracing::setup_trace( trace_data* trace_data, const math::vector3& start, const math::vector3& delta, const filter& filter, int penetration_count, bool trace_world ) const
	{
		memory::call<void>(PATTERN (patterns::trace_bullet_data_init), trace_data, start, delta, filter, penetration_count, trace_world );
	}

	void tracing::init_result( result* trace_result ) const
	{
		memory::call<void>(PATTERN (patterns::trace_bullet_free), trace_result );
	}

	void tracing::finalize_trace( trace_data* trace_data, result* hit, float unknown_float, void* unknown ) const
	{
		memory::call<void>(PATTERN (patterns::trace_bullet_update), trace_data, hit, unknown_float, unknown );
	}

} // namespace systems
