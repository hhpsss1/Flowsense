#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <utilities/cstypes.hpp>
#include <core/systems/systems.hpp>
#include <core/features/misc/misc.hpp>
#include <core/features/features.hpp>
#include <core/rendering/rendering.hpp>
#include <external/xdraw/xui/xui.hpp>
#include <core/settings.hpp>
#include <protection/game_addresses.hpp>
#include <d3d11.h>

namespace systems
{
	using namespace features::misc;

	// ---------------------------------------------------------------------------
	// SRV capture registry
	// ---------------------------------------------------------------------------

	namespace panorama
	{
		namespace
		{
			inline std::vector<srv_capture> g_captures{};
			inline std::mutex g_mtx{};
		}

		void add_capture( const char* name, const char* texture_name, std::size_t& out_index )
		{
			std::scoped_lock lock( g_mtx );

			for ( std::size_t i = 0; i < g_captures.size(); ++i )
			{
				if ( g_captures[ i ].texture_name &&
				     std::strcmp( g_captures[ i ].texture_name, texture_name ) == 0 )
				{
					out_index = i;
					return;
				}
			}

			g_captures.push_back( { name, texture_name, nullptr } );
			out_index = g_captures.size() - 1;
		}

		void on_resource_view( const char* texture_name, ID3D11ShaderResourceView* srv )
		{
			if ( !texture_name || !srv )
				return;

			std::scoped_lock lock( g_mtx );

			for ( auto& entry : g_captures )
			{
				if ( !entry.texture_name )
					continue;

				if ( std::strstr( texture_name, entry.texture_name ) == nullptr )
					continue;

				srv->AddRef();
				if ( entry.srv )
					entry.srv->Release();
				entry.srv = srv;

				logging::console::print( xs( "[model_preview] captured SRV for '{}' ({})" ),
					entry.name ? entry.name : "", texture_name );
				break;
			}
		}

		ID3D11ShaderResourceView* get_srv( std::size_t index )
		{
			std::scoped_lock lock( g_mtx );

			if ( index >= g_captures.size() )
				return nullptr;

			return g_captures[ index ].srv;
		}

		void shutdown()
		{
			std::scoped_lock lock( g_mtx );

			for ( auto& entry : g_captures )
			{
				if ( entry.srv )
				{
					entry.srv->Release();
					entry.srv = nullptr;
				}
			}

			g_captures.clear();
		}

	} // namespace panorama

	// ---------------------------------------------------------------------------
	// Panorama script that builds the agent preview panel.
	//
	// The panel renders into a composition layer named 'malv_agent_preview' which
	// we then capture from the GetResourceView hook and draw ourselves.
	//
	// Matching the loadout-preview pipeline:
	//   - animgraphcharactermode 'main-menu' + pose_sequence so the model plays the
	//     main menu idle animations instead of the rigid buy-menu pose.
	//   - EquipPlayerWithItem on the current weapon defindex.
	//   - PlaySequence fired ~0.2s after creation plus a keepAlive tick that flips
	//     the panel opacity so Panorama keeps submitting the layer to the renderer.
	//   - The panel stays at near-zero opacity; only the composition copy is used.
	// ---------------------------------------------------------------------------

	std::string model_preview::build_create_script( const std::string& model_path, std::int16_t def_index ) const
	{
		std::string model = model_path;
		if ( model.empty() )
			model = "agents/models/ctm_st6/ctm_st6_variantk.vmdl";

		// Choose a main-menu idle sequence that matches the faction prefix of the
		// resolved model (ct_* vs t_*), mirroring the loadout preview.
		const auto is_ct = model.find( "ct_" ) != std::string::npos || model.find( "ctm_" ) != std::string::npos;
		const auto is_knife = ( def_index == cstypes::item_definition_index::weapon_knife0 )
			|| ( def_index == cstypes::item_definition_index::weapon_knife1 )
			|| ( def_index == cstypes::item_definition_index::weapon_knife2 )
			|| ( def_index >= 500 && def_index <= 525 );

		const char* sequence = is_knife
			? ( is_ct ? "ct_main_menu_knife_idle" : "t_main_menu_knife_idle" )
			: ( is_ct ? "ct_main_menu_rifle_awp_lookat" : "t_main_menu_rifle02_idle_awp_galil" );

		// Braces must be doubled for std::format; every JS literal { / } appears
		// as {{ / }} below.
		return std::format(
			"(function(){{"
			"var old=$.GetContextPanel().FindChildInLayoutFile('{}');"
			"if(old){{old.DeleteAsync(0);}}"
			"var p=$.CreatePanel('MapPlayerPreviewPanel',$.GetContextPanel(),'{}',{{"
			"'require-composition-layer':true,"
			"'composition-layer-texture-name':'{}',"
			"'transparent-background':true,"
			"'pin-fov':'vertical',"
			"camera:'cam_loadoutmenu_{}',"
			"map:'ui/buy_menu',"
			"player:true,"
			"playermodel:'{}',"
			"playername:'vanity_character',"
			"animgraphcharactermode:'main-menu',"
			"'pose_sequence':'{}',"
			"mouse_rotate:false,"
			"sync_spawn_addons:true,"
			"csm_split_plane0_distance_override:'250.0',"
			"'clip-costumes':true,"
			"'weight-anim-graph':true,"
			"style:'width:640px; height:864px; vertical-align:top; horizontal-align:left; opacity:0.001;'"
			"}});"
			"if(!p){{return;}}"
			"try{{"
			"var itemId=BigInt(0xF000000000000000)|BigInt(0<<16)|BigInt({});"
			"p.EquipPlayerWithItem(itemId);"
			"p.SetReadyForDisplay(true);"
			"}}catch(e){{}}"
			"$.Schedule(0.22,function(){{"
			"try{{"
			"if(!p||(p.IsValid&&!p.IsValid()))return;"
			"if(p.SetReadyForDisplay)p.SetReadyForDisplay(true);"
			"if(p.PlaySequence)p.PlaySequence('{}');"
			"}}catch(e){{}}"
			"}});"
			"var keepFlip=false;"
			"var keepAlive=function(){{"
			"try{{"
			"if(!p||(p.IsValid&&!p.IsValid()))return;"
			"if(p.SetReadyForDisplay)p.SetReadyForDisplay(true);"
			"keepFlip=!keepFlip;"
			"p.style.opacity=keepFlip?'0.002':'0.001';"
			"$.Schedule(0.25,keepAlive);"
			"}}catch(e){{}}"
			"}};"
			"$.Schedule(0.25,keepAlive);"
			"}})();",
			k_panel_name, k_panel_name, k_texture_name,
			is_ct ? "ct" : "t",
			model, sequence, def_index, sequence );
	}

	// Removes the MalvAgentPreview panel from the context panel that created it.
	static const char k_sz_destroy_script[] =
		"(function(){"
		"var p=$.GetContextPanel().FindChildInLayoutFile('MalvAgentPreview');"
		"if(p){p.DeleteAsync(0);}"
		"})();";

	// ---------------------------------------------------------------------------
	// initialize
	// ---------------------------------------------------------------------------

	bool model_preview::initialize()
	{
		m_initialized       = true;
		m_script_injected   = false;
		m_capture_index     = 0;
		m_preview_panel     = nullptr;
		m_preview_player.store( 0 );

		logging::console::print( xs( "[model_preview] initialized" ) );
		return true;
	}

	// ---------------------------------------------------------------------------
	// run_script
	// ---------------------------------------------------------------------------

	void model_preview::run_script( c_ui_panel* panel, const char* script )
	{
		auto* panorama = reinterpret_cast<c_panorama_ui_engine*>( addresses::globals::panorama );
		if ( !panorama )
			return;

		auto* ui_engine = panorama->get_ui_engine(); // CUIEngineSource2 via vfunc 13
		if ( !ui_engine )
			return;

		ui_engine->run_script( panel, script ); // vfunc 77
	}

	// ---------------------------------------------------------------------------
	// find_script_panel
	// ---------------------------------------------------------------------------

	c_ui_panel* model_preview::find_script_panel()
	{
		// During a match the main-menu context idles and its 3D layers are no
		// longer presented, which freezes the preview copy. Prefer the HUD panel
		// (CsgoHudPanel) while in a match and the main menu otherwise.
		const auto in_match = g_local.get().is_valid();

		auto resolve_panel = [ & ]( std::uintptr_t base ) -> c_ui_panel*
			{
				if ( !base )
					return nullptr;

				const auto panel2d = memory::safe_read<std::uintptr_t>( base ).value_or( 0 );
				if ( !panel2d )
					return nullptr;

				auto* panel = memory::safe_read<c_ui_panel*>( panel2d + 0x8 ).value_or( nullptr );
				if ( !panel )
					return nullptr;

				const auto vtable = memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( panel ) ).value_or( 0 );
				if ( !vtable )
					return nullptr;

				return panel;
			};

		// Preference order: the context that is actually drawing right now.
		const auto preferred = in_match ? PATTERN( patterns::csgo_hud_panel ) : PATTERN( patterns::main_menu_panel );
		if ( auto* panel = resolve_panel( preferred ) )
			return panel;

		// Fall back to the other context when the preferred one is not alive yet
		// (e.g. HUD not spawned during map load, or main menu hidden in-match).
		const auto fallback = in_match ? PATTERN( patterns::main_menu_panel ) : PATTERN( patterns::csgo_hud_panel );
		if ( auto* panel = resolve_panel( fallback ) )
			return panel;

		logging::console::print( xs( "[model_preview] no script panel available" ) );
		return nullptr;
	}

	// ---------------------------------------------------------------------------
	// find_preview_panel
	//
	// Walks the children of the script panel looking for the CUIPanel backing our
	// MapPlayerPreviewPanel. Identified by its portrait_world pointer at +0x68
	// holding a readable world scene + the world_to_clip matrix at +0x2F8 being
	// finite. Result is cached and revalidated.
	// ---------------------------------------------------------------------------

	c_ui_panel* model_preview::find_preview_panel()
	{
		auto* script_panel = find_script_panel();
		if ( !script_panel )
			return nullptr;

		auto* result = static_cast<c_ui_panel*>( nullptr );

		std::vector<c_ui_panel*> stack{ script_panel };
		std::vector<c_ui_panel*> scratch{};

		while ( !stack.empty() )
		{
			auto* node = stack.back();
			stack.pop_back();

			if ( !node )
				continue;

			// CUI_Player3dPanel: CCS_PortraitWorld* at panel + 0x68.
			const auto portrait_world = memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( node ) + 0x68 ).value_or( 0 );
			if ( portrait_world )
			{
				const auto matrix = memory::safe_read<float>( reinterpret_cast<std::uintptr_t>( node ) + 0x2f8 ).value_or( 0.0f );
				if ( std::isfinite( matrix ) )
				{
					result = node;
					break;
				}
			}

			const auto child_count = memory::safe_read<std::uint32_t>( reinterpret_cast<std::uintptr_t>( node ) + 0x28 ).value_or( 0 );
			const auto children = memory::safe_read<c_ui_panel**>( reinterpret_cast<std::uintptr_t>( node ) + 0x30 ).value_or( nullptr );
			if ( !children || child_count > 0x100 )
				continue;

			for ( auto i = 0u; i < child_count; ++i )
			{
				auto* child = memory::safe_read<c_ui_panel*>( reinterpret_cast<std::uintptr_t>( children ) + static_cast<std::uintptr_t>( i ) * sizeof(c_ui_panel*) ).value_or( nullptr );
				if ( child && child != node )
				{
					scratch.push_back( child );
				}
			}

			for ( auto it = scratch.rbegin(); it != scratch.rend(); ++it )
				stack.push_back( *it );

			scratch.clear();
			if ( stack.size() > 2048 )
				break;
		}

		if ( result )
			logging::console::print( xs( "[model_preview] preview panel = {:p}" ), static_cast<void*>( result ) );

		return result;
	}

	// ---------------------------------------------------------------------------
	// find_preview_player
	//
	// The C_CSGO_PreviewPlayer spawned by the MapPlayerPreviewPanel is a regular
	// client entity (e.g. the buy-menu models), so it lives in the game's entity
	// list. The render hook path is not guaranteed to fire for panorama 3D layers,
	// so scan the list as a reliable source for the entity (and its bones).
	// Throttled by the caller; cheap once cached because only a handful of chunk
	// reads are needed to walk the whole list.
	// ---------------------------------------------------------------------------

	std::uintptr_t model_preview::find_preview_player() const
	{
		for ( auto i = 0; i < 2048; ++i )
		{
			const auto entity = g_entities.get_by_index( i );
			if ( !entity )
				continue;

			const auto name = g_entities.get_schema_name( entity );
			if ( !name || std::strstr( name, "PreviewPlayer" ) == nullptr )
				continue;

			return entity;
		}

		return 0;
	}

	// ---------------------------------------------------------------------------
	// resolve_model_path
	// ---------------------------------------------------------------------------

	std::string model_preview::resolve_model_path( int team ) const
	{
		// Prefer the changer's configured agent for the current team.
		const auto& agents_cfg = settings::g_changer.agents;
		const auto selected_def = ( team == 3 ) ? agents_cfg.ct_def : ( team == 2 ) ? agents_cfg.t_def : agents_cfg.ct_def;

		if ( selected_def != 0 )
		{
			if ( const auto* const def = features::changer::g_econ_item_system.find_def( selected_def ) )
			{
				if ( !def->model_player.empty() )
					return def->model_player;
			}
		}

		// Fallback: whatever the local pawn is currently wearing while in-match.
		const auto local = g_local.get();
		if ( local.pawn )
		{
			const auto game_scene_node = memory::safe_read<std::uintptr_t>( local.pawn + SCHEMA( "C_BaseEntity", "m_pGameSceneNode"_hash ) ).value_or( 0 );
			if ( game_scene_node )
			{
				const auto model_state = game_scene_node + SCHEMA( "CSkeletonInstance", "m_modelState"_hash );
				const auto model_name_ptr = memory::safe_read<std::uintptr_t>( model_state + SCHEMA( "CModelState", "m_ModelName"_hash ) ).value_or( 0 );
				if ( model_name_ptr )
				{
					const auto current = memory::read_string( model_name_ptr );
					if ( current.find( "agents/models" ) != std::string::npos )
						return current;
				}
			}
		}

		// Safety net: vanilla agent without the changers installed.
		return team == 2
			? "agents/models/tm_phoenix/tm_phoenix_variantg.vmdl"
			: "agents/models/ctm_st6/ctm_st6_variantk.vmdl";
	}

	// ---------------------------------------------------------------------------
	// resolve_def_index
	// ---------------------------------------------------------------------------

	std::int16_t model_preview::resolve_def_index() const
	{
		// Prefer the active weapon of the local pawn when in-match.
		const auto local = g_local.get();
		if ( local.pawn && local.is_alive )
		{
			const auto weapon_services = memory::safe_read<std::uintptr_t>( local.pawn + SCHEMA( "C_BasePlayerPawn", "m_pWeaponServices"_hash ) ).value_or( 0 );
			if ( weapon_services )
			{
				const auto active_handle = memory::safe_read<std::uint32_t>( weapon_services + SCHEMA( "CPlayer_WeaponServices", "m_hActiveWeapon"_hash ) ).value_or( 0 );
				if ( active_handle )
				{
					const auto weapon = g_entities.lookup( active_handle );
					if ( weapon )
					{
						const auto vdata = memory::safe_read<std::uintptr_t>( weapon + SCHEMA( "C_BaseEntity", "m_nSubclassID"_hash ) + 0x8 ).value_or( 0 );
						if ( vdata )
						{
							const auto name_ptr = memory::safe_read<const char*>( vdata + SCHEMA( "CCSWeaponBaseVData", "m_szName"_hash ) ).value_or( nullptr );
							if ( name_ptr )
							{
								auto name = memory::read_string( reinterpret_cast<std::uintptr_t>( name_ptr ), 64 );
								if ( name.starts_with( "weapon_" ) )
									name.erase( 0, 7 );

								for ( const auto& def : features::changer::g_econ_item_system.item_defs() )
								{
									if ( def.name.starts_with( "weapon_" ) )
									{
										if ( def.name.substr( 7 ) == name )
											return def.def_index;
									}
									else if ( def.name == name )
									{
										return def.def_index;
									}
								}
							}
						}
					}
				}
			}
		}

		// Menu fallback: server loadout is unavailable, use an AWP rifle so the
		// buy-menu main-menu animation is representative.
		return cstypes::item_definition_index::weapon_awp;
	}

	// ---------------------------------------------------------------------------
	// inject_panel / destroy_panel
	// ---------------------------------------------------------------------------

	void model_preview::inject_panel( bool force )
	{
		auto* script_panel = find_script_panel();
		if ( !script_panel )
			return;

		// Resolve the model + weapon we want to display. Re-inject whenever the
		// identity changes (agent swap, weapon switch) or the context panel the
		// preview lives under changed (main menu <-> match transition).
		const auto local = g_local.get();
		const auto team = ( local.team == 2 || local.team == 3 ) ? local.team : 3;
		const auto model_path = this->resolve_model_path( team );
		const auto def_index = this->resolve_def_index();

		const auto changed = ( m_script_panel != script_panel )
			|| ( m_last_model_path != model_path )
			|| ( m_last_def_index != def_index );

		if ( m_script_injected && !force && !changed )
			return;

		if ( m_script_injected )
		{
			// Tear down the previous instance so the new identity replaces it.
			// Destroy on the context that owns the old panel (may differ).
			const auto old_panel = m_script_panel ? m_script_panel : script_panel;
			run_script( old_panel, k_sz_destroy_script );
			m_script_injected = false;
		}

		panorama::add_capture( "model_preview", k_texture_name, m_capture_index );

		const auto script = this->build_create_script( model_path, def_index );
		run_script( script_panel, script.c_str() );

		m_last_model_path = model_path;
		m_last_def_index = def_index;
		m_script_panel = script_panel;
		m_script_injected = true;
		m_preview_panel = nullptr;
		m_preview_panel_scan_tick = 0;
		m_preview_player.store( 0 );

		logging::console::print( xs( "[model_preview] panorama panel injected (model='{}', def={}, panel={:p}, in_match={})" ),
			model_path, def_index, static_cast<void*>( script_panel ), local.is_valid() );
	}

	void model_preview::destroy_panel()
	{
		if ( auto* script_panel = m_script_panel ? m_script_panel : find_script_panel() )
			run_script( script_panel, k_sz_destroy_script );

		m_script_injected = false;
		m_script_panel = nullptr;
		m_preview_panel = nullptr;
		m_preview_player.store( 0 );
	}

	// ---------------------------------------------------------------------------
	// capture_preview_player
	//
	// Fast-path attempt from the GeneratePrimitives hook: when the pool pre-render
	// submits the preview model's scene object through the same path as regular
	// entities, this grabs the C_CSGO_PreviewPlayer immediately. When that path
	// does not run for the panorama layer, find_preview_player() covers it from
	// the entity list instead.
	// ---------------------------------------------------------------------------

	void model_preview::capture_preview_player( std::uintptr_t entity )
	{
		if ( !entity || !m_script_injected )
			return;

		const auto name = g_entities.get_schema_name( entity );
		if ( !name )
			return;

		// C_CSGO_PreviewPlayer subclasses C_CSPlayerPawn; the weapon meshes that
		// belong to the same scene object report the weapon entity instead.
		if ( std::strstr( name, "PreviewPlayer" ) == nullptr )
			return;

		const auto known = m_preview_player.load( std::memory_order_relaxed );
		if ( known == entity )
			return;

		m_preview_player.store( entity, std::memory_order_release );
		logging::console::print( xs( "[model_preview] preview player = {:p} ('{}')" ), reinterpret_cast<void*>( entity ), name );
	}

	// ---------------------------------------------------------------------------
	// get_srv
	// ---------------------------------------------------------------------------

	ID3D11ShaderResourceView* model_preview::get_srv() const
	{
		return panorama::get_srv( m_capture_index );
	}

	// ---------------------------------------------------------------------------
	// draw_esp_preview
	// ---------------------------------------------------------------------------

	void model_preview::draw_esp_preview( float x, float y, float w, float h, int subtab )
	{
		if ( !m_initialized )
			return;

		auto& dl = xui::draw::current();

		// inject_panel() is cheap and self-healing: without a force flag it no-ops
		// unless the script context changed (main menu <-> match) or the displayed
		// model / weapon changed. Never destroy an existing panel here, otherwise
		// the composition layer is torn down before its SRV ever binds.
		inject_panel();
		if ( !m_script_injected )
			return;

		auto* srv = get_srv();
		if ( !srv )
		{
			if ( ++m_capture_wait_tick >= 60 )
			{
				m_capture_wait_tick = 0;
				logging::console::print( xs( "[model_preview] SRV not captured yet (injected={}, panel={:p}, preview={:p})" ),
					m_script_injected, static_cast<void*>( m_preview_panel ), reinterpret_cast<void*>( m_preview_player.load( std::memory_order_relaxed ) ) );
			}

			dl.rect_filled( x, y, w, h, xdraw::color{ 20, 20, 20, 255 }, xdraw::corner_radius{ 8.0f } );
			dl.text( x + w * 0.5f - 50.0f, y + h * 0.5f - 8.0f, "Loading Preview...", xdraw::color{ 255, 255, 255, 150 } );
			return;
		}

		dl.push_clip( x, y, w, h );
		dl.image( x, y, w, h, srv, xdraw::corner_radius{ 8.0f } );
		dl.pop_clip();

		this->draw_preview_esp( dl, x, y, w, h, subtab );
	}

	// ---------------------------------------------------------------------------
	// draw_preview_esp
	//
	// Reads the finalized bones of the seated preview player, projects each joint
	// through the panel's world_to_clip matrix (a UV space over the captured
	// composition layer), maps UV back into the same ImGui rect the copy is drawn
	// into, and paints the enabled ESP elements (box, skeleton, health, weapon).
	// ---------------------------------------------------------------------------

	void model_preview::draw_preview_esp( xdraw::draw_list& dl, float x, float y, float w, float h, int subtab )
	{
		if ( !settings::g_esp.m_player.m_overlay[ 0 ].enabled.value && !settings::g_esp.m_player.m_overlay[ 1 ].enabled.value )
			return;

		const auto& cfg = settings::g_esp.m_player.m_overlay[ subtab ];
		if ( !cfg.enabled.value )
		{
			return;
		}

		if ( !m_preview_panel )
		{
			const auto idx = ++m_preview_panel_scan_tick;
			if ( ( idx & 7 ) == 0 )
				m_preview_panel = this->find_preview_panel();

			m_preview_player.store( 0 );
		}

		auto preview_player = m_preview_player.load( std::memory_order_acquire );
		if ( !preview_player )
		{
			// The GeneratePrimitives hook may never fire for the panorama 3D copy
			// (this is why chams cannot tint the preview either). Fall back to the
			// entity list so the ESP overlay still has a body to project.
			const auto idx = ++m_preview_panel_scan_tick;
			if ( ( idx & 7 ) == 0 )
			{
				preview_player = this->find_preview_player();
				if ( preview_player )
					logging::console::print( xs( "[model_preview] preview player via entity scan = {:p}" ), reinterpret_cast<void*>( preview_player ) );

				m_preview_player.store( preview_player, std::memory_order_release );
			}
		}

		if ( !preview_player || !m_preview_panel )
			return;

		// Keep the cached panel honest: re-resolve if the composition layer died.
		if ( !memory::safe_read<std::uintptr_t>( reinterpret_cast<std::uintptr_t>( m_preview_panel ) ).has_value() )
		{
			m_preview_panel = nullptr;
			return;
		}

		const auto skeleton = g_bones.get_skeleton( preview_player );
		if ( skeleton[ cstypes::bone_ids::head ].position.length_sqr( ) < 1.0f )
			return;

		// Client panel's world_to_clip (fallback +0x288). Output is a UV 0..1
		// over the captured composition layer.
		const auto panel = reinterpret_cast<std::uintptr_t>( m_preview_panel );
		auto matrix = memory::safe_read<math::matrix4x4>( panel + 0x2f8 );
		if ( !matrix || !std::isfinite( ( *matrix )[ 3 ][ 3 ] ) )
		{
			matrix = memory::safe_read<math::matrix4x4>( panel + 0x288 );
			if ( !matrix || !std::isfinite( ( *matrix )[ 3 ][ 3 ] ) )
				return;
		}

		auto project = [ & ]( const math::vector3& world, math::vector2& out ) -> bool
			{
				const auto& m = *matrix;

				const auto cw = m[ 3 ][ 0 ] * world.x + m[ 3 ][ 1 ] * world.y + m[ 3 ][ 2 ] * world.z + m[ 3 ][ 3 ];
				if ( cw < 0.001f || !std::isfinite( cw ) )
					return false;

				const auto cx = m[ 0 ][ 0 ] * world.x + m[ 0 ][ 1 ] * world.y + m[ 0 ][ 2 ] * world.z + m[ 0 ][ 3 ];
				const auto cy = m[ 1 ][ 0 ] * world.x + m[ 1 ][ 1 ] * world.y + m[ 1 ][ 2 ] * world.z + m[ 1 ][ 3 ];

				const auto inv = 1.0f / cw;
				const auto u = 0.5f * ( 1.0f + cx * inv );
				const auto v = 0.5f * ( 1.0f - cy * inv );

				out.x = x + u * w;
				out.y = y + v * h;
				return u >= -0.25f && u <= 1.25f && v >= -0.25f && v <= 1.25f;
			};

		constexpr auto none{ ~0u };
		constexpr std::array chains
		{
			std::array{ cstypes::bone_ids::head, cstypes::bone_ids::neck, cstypes::bone_ids::spine_4, cstypes::bone_ids::spine_3, cstypes::bone_ids::spine_2, cstypes::bone_ids::spine_1, cstypes::bone_ids::pelvis },
			std::array{ cstypes::bone_ids::left_hand, cstypes::bone_ids::left_elbow, cstypes::bone_ids::left_shoulder, cstypes::bone_ids::left_clavicle, cstypes::bone_ids::spine_4, none, none },
			std::array{ cstypes::bone_ids::right_hand, cstypes::bone_ids::right_elbow, cstypes::bone_ids::right_shoulder, cstypes::bone_ids::right_clavicle, cstypes::bone_ids::spine_4, none, none },
			std::array{ cstypes::bone_ids::left_foot, cstypes::bone_ids::left_knee, cstypes::bone_ids::left_hip, cstypes::bone_ids::pelvis, none, none, none },
			std::array{ cstypes::bone_ids::right_foot, cstypes::bone_ids::right_knee, cstypes::bone_ids::right_hip, cstypes::bone_ids::pelvis, none, none, none },
		};

		std::array<math::vector2, 27> screen{};
		std::array<bool, 27> valid{};

		for ( auto i = 0u; i < skeleton.size() && i < screen.size(); ++i )
		{
			if ( skeleton[ i ].position.length_sqr( ) < 1.0f )
				continue;

			valid[ i ] = project( skeleton[ i ].position, screen[ i ] );
		}

		if ( cfg.m_skeleton.enabled.value )
		{
			const auto& col = cfg.m_skeleton.visible_color.value;

			for ( const auto& chain : chains )
			{
				std::optional<math::vector2> prev{};

				for ( const auto& b : chain )
				{
					if ( b == none )
						break;

					if ( b >= valid.size() || !valid[ b ] )
					{
						prev.reset();
						continue;
					}

					if ( prev )
					{
						const auto dx = screen[ b ].x - prev->x;
						const auto dy = screen[ b ].y - prev->y;
						if ( dx * dx + dy * dy <= 90000.0f )
							dl.line( prev->x, prev->y, screen[ b ].x, screen[ b ].y, col, cfg.m_skeleton.thickness.value );
					}

					prev = screen[ b ];
				}
			}
		}

		if ( !cfg.m_box.enabled.value && !cfg.m_health_bar.enabled.value && !cfg.m_name.enabled.value && !cfg.m_weapon.enabled.value )
			return;

		// Bounding box from the projected joints.
		auto min = math::vector2{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
		auto max = math::vector2{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

		{
			std::array<math::vector2, 5> box_pts{};
			auto n{ 0 };
			const auto push = [ & ]( std::size_t idx )
				{
					if ( n < box_pts.size() && idx < valid.size() && valid[ idx ] )
						box_pts[ n++ ] = screen[ idx ];
				};

			push( cstypes::bone_ids::head );
			push( cstypes::bone_ids::neck );
			push( cstypes::bone_ids::pelvis );
			push( cstypes::bone_ids::left_foot );
			push( cstypes::bone_ids::right_foot );

			for ( auto i = 0; i < n; ++i )
			{
				if ( box_pts[ i ].x < min.x ) min.x = box_pts[ i ].x;
				if ( box_pts[ i ].y < min.y ) min.y = box_pts[ i ].y;
				if ( box_pts[ i ].x > max.x ) max.x = box_pts[ i ].x;
				if ( box_pts[ i ].y > max.y ) max.y = box_pts[ i ].y;
			}
		}

		if ( !( max.x > min.x ) || !( max.y > min.y ) )
			return;

		// Enlarge the head tip by a standard unit so the box clears the cap.
		min.y -= ( max.y - min.y ) * 0.075f;

		const auto corners = math::vector2{ min.x, min.y };
		const auto size = math::vector2{ max.x - min.x, max.y - min.y };

		if ( cfg.m_box.enabled.value )
		{
			const auto& col = cfg.m_box.visible_color.value;
			const auto bx = std::floorf( corners.x ), by = std::floorf( corners.y );
			const auto bw = std::floorf( size.x ), bh = std::floorf( size.y );

			if ( cfg.m_box.style == settings::esp::player::overlay::box::style_type::full )
			{
				auto outline = [ & ]( float ox, float oy, float ow, float oh, const xdraw::color& c, float t )
					{
						dl.line( ox, oy, ox + ow, oy, c, t );
						dl.line( ox + ow, oy, ox + ow, oy + oh, c, t );
						dl.line( ox + ow, oy + oh, ox, oy + oh, c, t );
						dl.line( ox, oy + oh, ox, oy, c, t );
					};

				if ( cfg.m_box.outline.value )
					outline( bx - 1, by - 1, bw + 2, bh + 2, xdraw::color{ 0, 0, 0, 180 }, 2.0f );

				outline( bx, by, bw, bh, col, 1.0f );
			}
			else
			{
				const auto corner_len = std::min( cfg.m_box.corner_length.value, std::min( bw, bh ) * 0.4f );

				auto outline = [ & ]( float ox, float oy, float ow, float oh, const xdraw::color& c, float t, float cl )
					{
						dl.line( ox, oy, ox + cl, oy, c, t );
						dl.line( ox, oy, ox, oy + cl, c, t );
						dl.line( ox + ow - cl, oy, ox + ow, oy, c, t );
						dl.line( ox + ow, oy, ox + ow, oy + cl, c, t );
						dl.line( ox, oy + oh - cl, ox, oy + oh, c, t );
						dl.line( ox, oy + oh, ox + cl, oy + oh, c, t );
						dl.line( ox + ow - cl, oy + oh, ox + ow, oy + oh, c, t );
						dl.line( ox + ow, oy + oh - cl, ox + ow, oy + oh, c, t );
					};

				if ( cfg.m_box.outline.value )
					outline( bx - 1, by - 1, bw + 2, bh + 2, xdraw::color{ 0, 0, 0, 180 }, 2.0f, corner_len + 1 );

				outline( bx, by, bw, bh, col, 1.0f, corner_len );
			}
		}

		auto health{ 100 };
		{
			const auto local = g_local.get();
			if ( local.pawn )
				health = memory::safe_read<int>( local.pawn + SCHEMA( "C_BaseEntity", "m_iHealth"_hash ) ).value_or( 100 );
		}

		if ( cfg.m_health_bar.enabled.value )
		{
			constexpr auto bar_size = 3.5f, padding = 4.0f;
			const auto clamped = std::clamp( health, 0, 100 );
			const auto fraction = static_cast< float >( clamped ) / 100.0f;
			const auto vertical = cfg.m_health_bar.position == settings::esp::player::overlay::health_bar::position_type::left;

			const auto bar_w = vertical ? bar_size : std::floorf( size.x );
			const auto bar_h = vertical ? std::floorf( size.y ) : bar_size;
			const auto filled = ( clamped >= 100 ) ? ( vertical ? bar_h : bar_w ) : std::floorf( ( vertical ? bar_h : bar_w ) * fraction );

			float box_x = 0.0f, box_y = 0.0f;
			switch ( cfg.m_health_bar.position )
			{
			case settings::esp::player::overlay::health_bar::position_type::left:
				box_x = std::floorf( corners.x - bar_size - padding );
				box_y = std::floorf( corners.y );
				break;
			case settings::esp::player::overlay::health_bar::position_type::top:
				box_x = std::floorf( corners.x );
				box_y = std::floorf( corners.y - bar_size - padding );
				break;
			case settings::esp::player::overlay::health_bar::position_type::bottom:
				box_x = std::floorf( corners.x );
				box_y = std::floorf( corners.y + size.y + padding );
				break;
			}

			if ( cfg.m_health_bar.outline_setting.value )
				dl.rect_filled( box_x - 1, box_y - 1, bar_w + 2, bar_h + 2, cfg.m_health_bar.outline_color.value );

			dl.rect_filled( box_x, box_y, bar_w, bar_h, cfg.m_health_bar.background_color.value );

			if ( filled > 0 )
			{
				if ( vertical )
					dl.rect_filled( box_x, box_y + bar_h - filled, bar_w, filled, cfg.m_health_bar.full_color.value );
				else
					dl.rect_filled( box_x, box_y, filled, bar_h, cfg.m_health_bar.full_color.value );
			}
		}

		std::string weapon_name;
		{
			const auto* const def = features::changer::g_econ_item_system.find_def( m_last_def_index );
			if ( def )
			{
				weapon_name = def->localized_name.empty() ? def->name : def->localized_name;
				if ( weapon_name.starts_with( "weapon_" ) )
					weapon_name.erase( 0, 7 );
			}
		}

		if ( cfg.m_name.enabled.value || cfg.m_weapon.enabled.value )
		{
			xdraw::push_font( rendering::g_fonts.inter_bold[ rendering::fonts::size::petite ] );
		}

		if ( cfg.m_name.enabled.value )
		{
			const auto local = g_local.get();
			std::string display;

			if ( local.controller )
			{
				const auto name_ptr = memory::safe_read<std::uintptr_t>( local.controller + SCHEMA( "CCSPlayerController", "m_sSanitizedPlayerName"_hash ) ).value_or( 0 );
				if ( name_ptr )
					display = memory::read_string( name_ptr, 128 );
			}

			if ( display.empty() )
				display = "Preview";

			const auto [text_w, text_h] = xdraw::measure_text( display );
			const auto tx = std::floorf( corners.x + ( size.x * 0.5f ) - ( text_w * 0.5f ) );
			const auto ty = std::floorf( corners.y - text_h - 2.0f );

			dl.text( tx, ty, display, cfg.m_name.color.value );
		}

		if ( cfg.m_weapon.enabled.value && !weapon_name.empty() )
		{
			const auto [text_w, text_h] = xdraw::measure_text( weapon_name );
			const auto tx = std::floorf( corners.x + ( size.x * 0.5f ) - ( text_w * 0.5f ) );
			const auto ty = std::floorf( corners.y + size.y + 2.0f );

			dl.text( tx, ty, weapon_name, cfg.m_weapon.text_color.value );
		}

		if ( cfg.m_name.enabled.value || cfg.m_weapon.enabled.value )
		{
			xdraw::pop_font();
		}
	}

} // namespace systems