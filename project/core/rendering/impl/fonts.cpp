#include <pch/pch.hpp>
#include <core/resources/fonts/inter.hpp>
#include <core/resources/fonts/inter-tight.hpp>
#include <core/resources/fonts/nunito.hpp>
#include "../rendering.hpp"

namespace rendering {

	void fonts::initialize( )
	{
		this->load_family( this->inter_medium, std::as_bytes( std::span{ resources::fonts::inter::regular } ), { 12.0f, 15.0f, 18.0f } );
		this->load_family( this->inter_bold, std::as_bytes( std::span{ resources::fonts::inter::bold } ), { 12.0f, 15.0f, 18.0f } );
		this->load_family( this->nunito_regular, std::as_bytes( std::span{ resources::fonts::nunito::regular } ), { 13.0f, 16.0f, 20.0f } );
		this->load_family( this->inter_tight, std::as_bytes( std::span{ resources::fonts::inter_tight::bold } ), { 12.0f, 15.0f, 18.0f } );
	}

	void fonts::load_family( family_t& family, std::span<const std::byte> data, const std::array<float, static_cast< std::size_t >( size::count )>& sizes )
	{
		for ( auto i = 0ull; i < sizes.size( ); ++i )
		{
			family.sizes[ i ] = xdraw::load_font( data, sizes[ i ] );
		}
	}

} // namespace rendering
