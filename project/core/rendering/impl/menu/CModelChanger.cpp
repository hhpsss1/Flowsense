#include <pch/pch.hpp>
#include "CModelChanger.hpp"

#include <Windows.h>
#include <filesystem>
#include <algorithm>

#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <core/systems/systems.hpp>
#include <protection/game_addresses.hpp>

namespace fs = std::filesystem;

namespace
{
	struct buffer_string_t
	{
		int length{};
		int allocated{ static_cast< int >( 0xC0000008u ) };
		char* ptr{ nullptr };
	};

	using create_interface_fn = void* ( * )( const char*, int* );

	std::uint8_t* find_pattern( const char* module_name, const char* pattern )
	{
		const auto module = GetModuleHandleA( module_name );
		if ( !module )
		{
			return nullptr;
		}

		const auto dos = reinterpret_cast< IMAGE_DOS_HEADER* >( module );
		if ( dos->e_magic != IMAGE_DOS_SIGNATURE )
		{
			return nullptr;
		}

		const auto nt = reinterpret_cast< IMAGE_NT_HEADERS64* >( reinterpret_cast< std::uint8_t* >( module ) + dos->e_lfanew );

		auto section = IMAGE_FIRST_SECTION( nt );
		for ( auto i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section )
		{
			if ( section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE || !( section->Characteristics & IMAGE_SCN_CNT_CODE ) )
			{
				continue;
			}

			const auto base = reinterpret_cast< std::uint8_t* >( module ) + section->VirtualAddress;
			const auto size = section->Misc.VirtualSize;

			for ( std::size_t j = 0; j < size; ++j )
			{
				auto matched = true;

				for ( std::size_t k = 0; pattern[ k ] != '\0'; ++k )
				{
					const auto c = pattern[ k ];

					if ( c == ' ' )
					{
						continue;
					}

					if ( c == '?' )
					{
						continue;
					}

					if ( c != '?' && ( pattern[ k + 1 ] == '\0' || pattern[ k + 1 ] == ' ' ) )
					{
						std::uint8_t byte{};

						if ( c >= '0' && c <= '9' )
						{
							byte = static_cast< std::uint8_t >( c - '0' );
						}
						else if ( c >= 'A' && c <= 'F' )
						{
							byte = static_cast< std::uint8_t >( c - 'A' + 10 );
						}
						else if ( c >= 'a' && c <= 'f' )
						{
							byte = static_cast< std::uint8_t >( c - 'a' + 10 );
						}

						if ( base[ j ] != byte )
						{
							matched = false;
							break;
						}

						continue;
					}

					char hex[ 3 ]{ c, pattern[ k + 1 ], '\0' };
					const auto byte = static_cast< std::uint8_t >( std::strtoul( hex, nullptr, 16 ) );

					if ( base[ j ] != byte )
					{
						matched = false;
						break;
					}

					++k;
				}

				if ( matched )
				{
					return base + j;
				}
			}
		}

		return nullptr;
	}
}

auto CModelChanger::OnInit() -> void
{
	if ( m_bInitialized )
	{
		return;
	}

	if ( const auto resource_system = GetModuleHandleA( "resourcesystem.dll" ) )
	{
		if ( const auto create_interface = reinterpret_cast< create_interface_fn >( GetProcAddress( resource_system, "CreateInterface" ) ) )
		{
			m_IRS = create_interface( "ResourceSystem013", nullptr );
		}
	}

	if ( const auto addr = find_pattern( "resourcesystem.dll", "40 53 55 57 48 81 EC 80 00 00 00 48 8B 01 49 8B E8 48 8B FA" ) )
	{
		m_fnPrecache = reinterpret_cast< void* ( * )( void*, void*, const char* ) >( addr );
	}

	if ( const auto tier0 = GetModuleHandleA( "tier0.dll" ) )
	{
		m_fnInsert = reinterpret_cast< const char* ( __fastcall* )( void*, int, const char*, int, bool ) >(
			GetProcAddress( tier0, "?Insert@CBufferString@@QEAAPEBDHPEBDH_N@Z" ) );
	}

	ScanModels();

	m_bInitialized = true;
}

auto CModelChanger::ScanModels() -> void
{
	m_Models.clear();
	m_Models.push_back( { "[ OFF ]", "" } );

	char exe_path[ MAX_PATH ]{};
	GetModuleFileNameA( nullptr, exe_path, MAX_PATH );

	std::string root = exe_path;

	auto pos = root.find( "bin\\win64" );
	if ( pos != std::string::npos )
	{
		root.replace( pos, 9, "csgo\\characters\\models" );
	}
	else
	{
		char cwd[ MAX_PATH ]{};
		GetCurrentDirectoryA( MAX_PATH, cwd );
		root = std::string( cwd ) + "\\csgo\\characters\\models";
	}

	if ( !fs::exists( root ) )
	{
		return;
	}

	for ( const auto& p : fs::recursive_directory_iterator( root ) )
	{
		if ( p.path().extension() != ".vmdl_c" )
		{
			continue;
		}

		std::string full = p.path().string();
		const auto ch_pos = full.find( "characters\\" );
		if ( ch_pos == std::string::npos )
		{
			continue;
		}

		std::string rel = full.substr( ch_pos );
		std::replace( rel.begin(), rel.end(), '\\', '/' );
		rel = rel.substr( 0, rel.find( ".vmdl_c" ) ) + ".vmdl";

		const auto fname = p.path().stem().string();
		auto fname_low = fname;
		std::transform( fname_low.begin(), fname_low.end(), fname_low.begin(), []( unsigned char c ) { return static_cast< char >( std::tolower( c ) ); } );

		if ( fname_low.find( "_arm" ) != std::string::npos ||
			fname_low.find( "_hand" ) != std::string::npos ||
			fname_low.find( "_glove" ) != std::string::npos ||
			fname_low.find( "hitbox" ) != std::string::npos )
		{
			continue;
		}

		m_Models.push_back( { fname, rel } );
	}
}

auto CModelChanger::PrecacheResource( const std::string& path ) -> void
{
	if ( !m_fnPrecache || !m_IRS || !m_fnInsert )
	{
		return;
	}

	buffer_string_t names{};
	m_fnInsert( &names, 0, path.c_str(), -1, false );
	m_fnPrecache( m_IRS, &names, "" );
}

auto CModelChanger::ChangeModelNow() -> void
{
	if ( m_SelectedIdx <= 0 || m_SelectedIdx >= static_cast< int >( m_Models.size() ) )
	{
		return;
	}

	const auto& path = m_Models[ m_SelectedIdx ].m_Path;
	if ( path.empty() )
	{
		return;
	}

	const auto local = systems::g_local.get();
	if ( !local.pawn )
	{
		return;
	}

	PrecacheResource( path );
	memory::call<void>( PATTERN( patterns::set_player_model ), local.pawn, path.c_str() );

	const auto collision = local.pawn + SCHEMA( "C_BaseModelEntity", "m_Collision"_hash );
	memory::write<math::vector3>( collision + SCHEMA( "CCollisionProperty", "m_vecMins"_hash ), math::vector3( -16.0f, -16.0f, 0.0f ) );
	memory::write<math::vector3>( collision + SCHEMA( "CCollisionProperty", "m_vecMaxs"_hash ), math::vector3( 16.0f, 16.0f, 72.0f ) );

	m_NeedSetModel = false;
}

auto CModelChanger::OnFrameStageNotify( int frame_stage ) -> void
{
	if ( frame_stage == 6 && m_NeedSetModel )
	{
		ChangeModelNow();
	}
}

auto GetModelChanger() -> CModelChanger*
{
	static CModelChanger instance{};
	return &instance;
}
