#pragma once

#include <filesystem>
#include <utilities/proto/proto.hpp>
#include <core/settings.hpp>

namespace features::misc { class c_ui_panel; }

namespace systems {

	class schemas
	{
	public:
		[[nodiscard]] static std::uint32_t lookup( const char* class_name, std::uint32_t field_hash );
	};

	class materials
	{
	public:
		enum class clone_type : int { translucent, ignorez };

		[[nodiscard]] static bool initialize( );
		[[nodiscard]] static std::uintptr_t find( settings::esp::cham_ids id );
		[[nodiscard]] static const char* get_texture_path( std::uintptr_t entry );
		[[nodiscard]] static std::string emit_translucent_kv( std::uintptr_t src_mat );
		[[nodiscard]] static std::string emit_ignorez_kv( std::uintptr_t src_mat );
		[[nodiscard]] static std::uintptr_t load( const char* vmat_data, const char* name );

		[[nodiscard]] static std::uintptr_t get_or_create_clone( std::uintptr_t src_mat, clone_type type = clone_type::translucent );
		static void clear_clones( );
		static void set_material_vec3( std::uintptr_t mat, const char* param_name, float x, float y, float z );

	private:
		static inline std::array<std::uintptr_t, static_cast< std::size_t >( settings::esp::cham_ids::count )> m_loaded{};
		static inline std::unordered_map<std::uint64_t, std::uintptr_t> m_map{};
		static inline std::vector<cstypes::strong_handle> m_handles{};
		static inline std::mutex m_mtx{};
	};

	class events
	{
	public:
		using handler_fn = void( __fastcall* )( void* event );

		static bool initialize( );
		static void shutdown( );

		static bool register_listener( const char* event_name, handler_fn handler );
		static void unregister_listener( const char* event_name );

	private:
		struct listener
		{
			void** vtable;
			int debug_id;
		};

		struct entry
		{
			listener listener;
			void* vtable_data[ 3 ];
			handler_fn handler;
			const char* name;
			bool registered;
		};

		static void* __fastcall fire_event( void* self, void* event );
		static int __fastcall get_debug_id( void* self );
		inline static std::vector<std::unique_ptr<entry>> m_listeners{};
	};

	class input
	{
	public:
		struct in_button_state
		{
			void* vtable;
			std::uintptr_t value;
			std::uintptr_t value_changed;
			std::uintptr_t value_scroll;
		};

		struct usercmd
		{
			void* vtable;
			std::intptr_t command_number;
			void* proto_vtable;
			void* proto_arena;
			proto::csgo_usercmd_pb csgo_user_cmd;
			in_button_state buttons;
			char pad1[ 0x8 ];
			double last_server_time;
			bool has_been_predicted;
			int flag;
			int command_type;
		};

		struct input_history_params
		{
			math::vector3 view_angles{};
			math::vector3 shoot_position{};
			int render_tick{};
			float render_frac{};
			int player_tick{};
			float player_frac{};
			int frame_number{};
			int target_ent_index{ -1 };

			float cl_interp_frac{};
			int sv_interp0_src{};
			int sv_interp0_dst{};
			float sv_interp0_frac{};
			int sv_interp1_src{};
			int sv_interp1_dst{};
			float sv_interp1_frac{};
			int player_interp_src{};
			int player_interp_dst{};
			float player_interp_frac{};

			math::vector3 target_head_pos{};
			math::vector3 target_abs_pos{};
			math::vector3 target_abs_ang{};
			bool fill_cheat_check_data{};
		};

		void update( );
		// suppress_move_alignment: skip the ag2 alignment delta. While
		// antiaim re-bases movement onto fake viewangles the delta would
		// inject the whole rotation difference as an extra movement impulse.
		void apply( bool suppress_move_alignment = false );

		[[nodiscard]] usercmd* get( ) const { return this->m_current_cmd; }
		[[nodiscard]] usercmd* get_current_cmd( std::uintptr_t local_controller ) const;
		[[nodiscard]] proto::subtick_move_step* acquire_subtick_step( proto::repeated_ptr_field<proto::subtick_move_step>* subtick_moves ) const;
		[[nodiscard]] math::vector3 get_view_angles( ) const;
		[[nodiscard]] proto::input_history_entry* push_input_history( usercmd* cmd, const input_history_params& params ) const;

		void set_view_angles( const math::vector3& angles ) const;
		void desubtick( usercmd* cmd ) const;
		void set_weapon_select( usercmd* cmd, std::uintptr_t csgo_input ) const;

		[[nodiscard]] usercmd* get_command_by_sequence( std::uintptr_t local_controller, int sequence ) const;
		[[nodiscard]] bool is_subtick_overwrite( usercmd* cmd ) const;

	private:
		usercmd* m_current_cmd{};
		proto::base_usercmd_pb m_backup{};

		bool calculate_crc( proto::base_usercmd_pb* base ) const;
	};

	class legit_input
	{
	public:
		//void press( std::uintptr_t button_bit, float when = -1.0f );
		//void release( std::uintptr_t button_bit, float when = -1.0f );
		void add_mouse_delta( float pitch_degrees, float yaw_degrees );

		//void on_create_move( input::usercmd* cmd );
		void on_process_input_event( std::uintptr_t csgo_input, int slot );

	private:
		//[[nodiscard]] float resolve_when( ) const;

		bool m_pending_press{};
		bool m_pending_release{};
		std::uintptr_t m_press_button{};
		std::uintptr_t m_release_button{};
		float m_press_when{ -1.0f };
		float m_release_when{ -1.0f };

		float m_pending_pitch{};
		float m_pending_yaw{};
	};

	class entities
	{
	public:
		enum class type : std::uint8_t
		{
			unknown,
			player,
			item,
			projectile
		};

		struct cached
		{
			std::uintptr_t ptr{};
			std::uint32_t schema_hash{};
			std::int16_t index{};
			type type{};
		};

		void on_add_entity( std::uintptr_t entity, std::uint32_t handle );
		void on_remove_entity( std::uintptr_t entity, std::uint32_t handle );
		void force_update( );
		void clear( );

		[[nodiscard]] bool exists( std::uintptr_t entity ) const;
		[[nodiscard]] const char* get_schema_name( std::uintptr_t entity ) const;
		[[nodiscard]] std::uintptr_t get_by_index( int index );
		[[nodiscard]] std::uintptr_t lookup( std::uint32_t handle ) const;
		[[nodiscard]] std::vector<cached> get_by_type( type type ) const;

		[[nodiscard]] bool is_empty( ) const;

	private:
		[[nodiscard]] type classify_entity( std::uint32_t schema_hash ) const;

		std::vector<cached> m_cached{};
		mutable std::shared_mutex m_cache_mtx{};
		std::array<std::uintptr_t, 32> m_cached_list_entries{};
		std::uintptr_t m_cached_entity_list{};
	};

	class local
	{
	public:
		struct snapshot
		{
			std::uintptr_t controller{};
			std::uintptr_t pawn{};
			std::uintptr_t observer_pawn{};
			std::uintptr_t observer_controller{};
			int team{};
			int view_team{};
			bool is_alive{};
			bool is_team_mode{};

			[[nodiscard]] std::uintptr_t view_controller( ) const { return this->is_alive ? this->controller : this->observer_controller; }
			[[nodiscard]] std::uintptr_t view_pawn( ) const { return this->is_alive ? this->pawn : this->observer_pawn; }
			[[nodiscard]] bool is_valid( ) const { return this->pawn != 0 || this->observer_pawn != 0; }
			[[nodiscard]] bool is_this_other_team( int other_team ) const { return !this->is_team_mode || this->view_team != other_team; }
		};

		void update( );

		[[nodiscard]] snapshot get( ) const
		{
			std::shared_lock lock( this->m_mtx );
			return this->m_snapshot;
		}

		[[nodiscard]] bool is_in_cinematic( ) const { return this->m_is_in_cinematic.load( ); }
		[[nodiscard]] bool is_in_time_freeze( ) const { return this->m_is_in_time_freeze.load( ); }
		[[nodiscard]] bool is_in_deathmatch( ) const { return this->m_is_deathmatch.load( ); }

		// made this public, to be called by level_shutdown
		void reset( );

	private:
		snapshot m_snapshot{};
		mutable std::shared_mutex m_mtx{};

		std::atomic<bool> m_is_deathmatch{};
		std::atomic<bool> m_is_in_cinematic{};
		std::atomic<bool> m_is_in_time_freeze{};
	};

	class prediction
	{
	public:
		struct state
		{
			std::uint32_t flags{};
			math::vector3 networked_velocity{};
			math::vector3 velocity{};
			math::vector3 origin{};
			math::vector3 networked_origin{};
			math::vector3 last_movement_impulses{};
			float surface_friction{};
			float stamina{};
		};

		void capture_prestate( std::uintptr_t local_pawn, std::uintptr_t movement_services );
		bool simulate( input::usercmd* cmd, const systems::local::snapshot& local, const std::function<void( )>& fn );

		[[nodiscard]] const state& pre( ) const { return this->m_prestate; }

	private:
		state m_prestate{};
		std::mutex m_simulation_mtx{};
	};

	class view
	{
	public:
		struct projection
		{
			math::vector2 screen{};
			float w{};
			bool on_screen{};
		};

		void update( std::uintptr_t view );
		void update_matrix( );

		[[nodiscard]] math::vector2 project( const math::vector3& world_pos );
		[[nodiscard]] projection project_full( const math::vector3& world_pos ) const;
		[[nodiscard]] bool projection_valid( const math::vector2& screen_pos ) { return screen_pos.x != this->k_invalid && screen_pos.y != this->k_invalid; }

		[[nodiscard]] bool has_camera( ) const { return this->m_matrix_valid.load( std::memory_order_acquire ); }
		[[nodiscard]] math::vector3 origin( ) const { return this->m_origin; }
		[[nodiscard]] math::vector3 angles( ) const { return this->m_angles; }
		[[nodiscard]] float fov( ) const { return this->m_fov; }
		[[nodiscard]] math::matrix4x4 matrix( ) const;

	private:
		static constexpr auto k_invalid{ 0xdead };

		math::matrix4x4 m_matrix{};
		mutable std::mutex m_matrix_mtx{};
		std::atomic<bool> m_matrix_valid{};
		math::vector3 m_origin{};
		math::vector3 m_angles{};
		float m_fov{ k_invalid };
	};

	class bones
	{
	public:
		struct data
		{
			math::vector3 position{};
			float scale{};
			math::quaternion rotation{};
		};

		[[nodiscard]] data get( std::uintptr_t entity, std::uint32_t bone_id );
		[[nodiscard]] std::array<data, 27> get_skeleton( std::uintptr_t entity );

	private:
		[[nodiscard]] std::uintptr_t get_bone_cache( std::uintptr_t entity, int* out_count = nullptr );
	};

	class bounds
	{
	public:
		struct data
		{
			math::vector2 min{};
			math::vector2 max{};
			bool valid{};

			[[nodiscard]] float width( ) const { return this->max.x - this->min.x; }
			[[nodiscard]] float height( ) const { return this->max.y - this->min.y; }
			[[nodiscard]] math::vector2 center( ) const { return { this->min.x + this->width( ) * 0.5f, this->min.y + this->height( ) * 0.5f }; }
		};

		[[nodiscard]] data get( std::uintptr_t entity );
	};

	class hitboxes
	{
	public:
		struct entry
		{
			int index{ -1 };
			int bone{ -1 };
			math::vector3 mins{};
			math::vector3 maxs{};
			float radius{};
			std::uint8_t shape_type{};
			bool translation_only{};
		};

		struct set
		{
			std::array<entry, 20> entries{};
			int count{};

			[[nodiscard]] const entry* begin( ) const { return this->entries.data( ); }
			[[nodiscard]] const entry* end( ) const { return this->entries.data( ) + this->count; }
		};

		[[nodiscard]] set query( std::uintptr_t game_scene_node, bool seh = false );
		[[nodiscard]] int hitgroup_from_hitbox( int hitbox );
		[[nodiscard]] const char* hitgroup_to_name( int hitgroup );
	};

	class tracing
	{
	public:
		struct filter
		{
			std::byte data[ 0x48 ];
		};

		static_assert( sizeof( filter ) == 0x48 );

		struct ray
		{
			math::vector3 mins;
			math::vector3 maxs;
			std::byte pad0[ 0x10 ];
			std::uint8_t type;
			std::byte pad1[ 0x7 ];
		};

		struct result
		{
			void* surface;                 // 0x00 m_pSurface
			std::uintptr_t hit_entity;     // 0x08 m_pHitEntity
			void* hitbox_data;             // 0x10 m_pHitboxData
			std::byte pad0[ 0x60 ];        // 0x18
			math::vector3 start_pos;       // 0x78
			math::vector3 end_pos;         // 0x84
			math::vector3 normal;          // 0x90
			std::byte pad1[ 0xc ];         // 0x9C
			float distance;                // 0xA8
			float fraction;                // 0xAC
			void* body;                    // 0xB0 m_pHitPhysObj
			std::uint16_t surface_flags;   // 0xB8 m_nSurfaceFlags
			std::uint8_t trace_type;       // 0xBA m_nTraceType
			bool all_solid;                // 0xBB m_bAllSolid
		};

		struct trace_array_element
		{
			std::byte pad0[ 0x38 ];
		};

		struct trace_data
		{
			int unknown1;                     // 0x0000 mod_count
			float unknown2{ 52.0f };          // 0x0004
			void* array_pointer;              // 0x0008 mod_data
			int unknown3{ 128 };              // 0x0010 mod_alloc
			int unknown4{ static_cast< int >( 0x80000000 ) }; // 0x0014 mod_flags
			std::array<trace_array_element, 0x80> elements;   // 0x0018 mod_inline
			std::byte pad0[ 0x8 ];            // 0x1c18
			int num_hits;                     // 0x1c20 arr_count
			int unknown5;                     // 0x1c24
			void* hit_array_pointer;          // 0x1c28 arr_data
			int hit_capacity{ 8 };            // 0x1c30 arr_alloc
			int hit_flags{ static_cast< int >( 0x80000000 ) }; // 0x1c34 arr_flags
			std::byte hit_elements[ 0x1c0 ];  // 0x1c38 arr_inline
			std::byte tail[ 0x208 ];          // 0x1df8 -> 0x2000
		};

		static_assert( offsetof( trace_data, elements ) == 0x18 );
		static_assert( offsetof( trace_data, num_hits ) == 0x1c20 );
		static_assert( offsetof( trace_data, hit_array_pointer ) == 0x1c28 );
		static_assert( sizeof( trace_data ) == 0x2000 );

		struct player_movement_filter
		{
			std::byte data[ 0x48 ];
		};

		struct bbox_collision
		{
			math::vector3 mins;
			math::vector3 maxs;
		};

		[[nodiscard]] bool is_visible( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, std::uintptr_t skip_entity, std::uintptr_t mask = 0x1c3003 ) const;

		[[nodiscard]] result trace( const math::vector3& start, const math::vector3& end, std::uintptr_t skip_entity, std::uintptr_t mask = 0x1c3003, std::uint8_t layer = 4 ) const;
		[[nodiscard]] result trace( const math::vector3& start, const math::vector3& end, const filter& filter ) const;
		[[nodiscard]] result trace_hull( const math::vector3& start, const math::vector3& end, const math::vector3& mins, const math::vector3& maxs, std::uintptr_t skip_entity, std::uintptr_t mask = 0x1c3003, std::uint8_t layer = 4 ) const;
		[[nodiscard]] result trace_hull( const math::vector3& start, const math::vector3& end, const math::vector3& mins, const math::vector3& maxs, const filter& filter ) const;
		[[nodiscard]] result trace_sphere( const math::vector3& start, const math::vector3& end, float radius, const filter& filter ) const;
		[[nodiscard]] result trace_to_entity( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, std::uintptr_t skip_entity, std::uintptr_t mask = 0x1c3003, std::uint8_t layer = 4 ) const;
		[[nodiscard]] result trace_to_entity( const math::vector3& start, const math::vector3& end, std::uintptr_t target_entity, const filter& filter ) const;
		[[nodiscard]] filter make_filter( std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer, int type ) const;
		[[nodiscard]] filter make_filter( std::uintptr_t skip_entity, std::uintptr_t mask, std::uint8_t layer ) const;
		[[nodiscard]] player_movement_filter make_player_movement_filter( std::uintptr_t entity, std::uintptr_t mask, std::uint8_t collision_group = 11 ) const;
		[[nodiscard]] tracing::result trace_player_bbox( const math::vector3& start, const math::vector3& end, const bbox_collision& bbox, const player_movement_filter& filter, std::uintptr_t movement_services ) const;

		void setup_trace( trace_data* trace_data, const math::vector3& start, const math::vector3& delta, const filter& filter, int penetration_count, bool trace_world = false ) const;
		void init_result( result* trace_result ) const;
		void finalize_trace( trace_data* trace_data, result* trace_result, float unknown_float, void* unknown ) const;
	};

	class frame_data
	{
	public:
		void update( );

		[[nodiscard]] math::vector3 origin( ) const { return this->m_origin; }
		[[nodiscard]] bool valid( ) const { return this->m_valid; }

	private:
		void reset( );

		math::vector3 m_origin{};
		bool m_valid{};
	};

	class icons
	{
	public:
		struct icon
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture{};
			int width{};
			int height{};
		};

		bool initialize( );
		void shutdown( );

		[[nodiscard]] const icon* get( const std::string& name, float scale = 1.0f );
		[[nodiscard]] const icon* get( std::uint32_t schema_hash, float scale = 1.0f );

	private:
		bool load_vpk_directory( const std::filesystem::path& path );
		bool cache_svg_bytes( const std::filesystem::path& vpk_path, const std::string& icon_name, std::uint32_t entry_offset, std::uint32_t entry_length );
		std::vector<std::byte> decompile_vsvg( std::span<const std::byte> data ) const;

		struct icon_key
		{
			std::string name;
			std::uint32_t scale_bits;

			bool operator==( const icon_key& o ) const noexcept { return this->scale_bits == o.scale_bits && this->name == o.name; }
		};

		struct icon_key_hash
		{
			std::size_t operator()( const icon_key& k ) const noexcept { return std::hash<std::string>{}( k.name ) ^ ( std::hash<std::uint32_t>{}( k.scale_bits ) << 1 ); }
		};

		std::unordered_map<icon_key, icon, icon_key_hash> m_icons{};
		std::unordered_map<std::uint32_t, std::string> m_hash_to_name{};
		std::unordered_map<std::string, std::vector<std::byte>> m_pending_svgs{};
	};

	namespace panorama {

		// A registered capture: the composition-layer texture name we want and the
		// SRV that the GetResourceView hook hands us once it becomes available.
		struct srv_capture {
			const char* name = nullptr;
			const char* texture_name = nullptr;
			ID3D11ShaderResourceView* srv = nullptr;
		};

		// Registers a texture name to capture. Fills out_index with the capture slot.
		void add_capture( const char* name, const char* texture_name, std::size_t& out_index );
		// Called from the resource-view hook with every SRV the renderer binds.
		void on_resource_view( const char* texture_name, ID3D11ShaderResourceView* srv );
		// Returns the captured SRV for a registered capture slot.
		ID3D11ShaderResourceView* get_srv( std::size_t index );
		// Releases every captured SRV and clears the registry.
		void shutdown( );

	} // namespace panorama

	class model_preview
	{
	private:
		static constexpr const char* k_panel_name   = "MalvAgentPreview";
		static constexpr const char* k_texture_name = "malv_agent_preview";

		bool m_initialized = false;
		bool m_script_injected = false;
		std::size_t m_capture_index = 0;

		// The context panel (MainMenuPanel or CsgoHudPanel) the create script was
		// last run against. Changing contexts (main menu <-> match) forces a fresh
		// injection into the new context so the preview keeps rendering.
		features::misc::c_ui_panel* m_script_panel {};

		// The CUIPanel backing the MapPlayerPreviewPanel we created. Resolved once
		// and validated on re-entry. Used to read portrait_world and world_to_clip.
		features::misc::c_ui_panel* m_preview_panel {};
		std::uintptr_t m_preview_panel_scan_tick {};
		std::uintptr_t m_capture_wait_tick {};

		// The C_CSGO_PreviewPlayer spawned by that panel. Pushed from the
		// GeneratePrimitives hook (render-side). Guarded by an atomic; drawn only
		// after the bone cache has been finalized by the engine.
		std::atomic<std::uintptr_t> m_preview_player {};

		std::string m_last_model_path {};
		std::int16_t m_last_def_index {};

		// IPanoramaUIEngine->GetUIEngineSource2()->RunScript(CUIPanel*, script, origin, line)
		void run_script( features::misc::c_ui_panel* panel, const char* script );
		[[nodiscard]] features::misc::c_ui_panel* find_script_panel();
		[[nodiscard]] features::misc::c_ui_panel* find_preview_panel();
		void inject_panel( bool force = false );
		void destroy_panel();

		[[nodiscard]] std::uintptr_t find_preview_player() const;
		[[nodiscard]] std::string resolve_model_path( int team ) const;
		[[nodiscard]] std::int16_t resolve_def_index() const;
		[[nodiscard]] std::string build_create_script( const std::string& model_path, std::int16_t def_index ) const;
		void draw_preview_esp( xdraw::draw_list&, float x, float y, float w, float h, int subtab );

	public:
		bool initialize();
		void capture_preview_player( std::uintptr_t entity );

		[[nodiscard]] ID3D11ShaderResourceView* get_srv() const;

		void draw_esp_preview( float x, float y, float w, float h, int subtab );
	};

	inline input g_input{};
	inline legit_input g_legit_input{};
	inline entities g_entities{};
	inline local g_local{};
	inline prediction g_prediction{};
	inline view g_view{};
	inline bones g_bones{};
	inline bounds g_bounds{};
	inline hitboxes g_hitboxes{};
	inline tracing g_tracing{};
	inline frame_data g_frame_data{};
	inline icons g_icons{};
	inline model_preview g_model_preview{};

} // namespace systems

#define SCHEMA( class_name, field_hash ) \
	[]( ) -> int { \
		static const auto val = systems::schemas::lookup( class_name, field_hash ); \
		return val; \
	}( )
