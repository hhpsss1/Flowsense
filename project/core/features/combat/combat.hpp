#pragma once

#include <core/systems/systems.hpp>

namespace features::combat {

	// Ticks after landing during which the jump inaccuracy is still decaying
	// even though FL_ONGROUND is already set. Shots inside this window are
	// never considered max-accuracy so the ragebot does not dump a bullet
	// into spread right after a jump lands.
	inline constexpr auto k_land_recover_ticks{ 12 };

	class shared
	{
	public:
		// Damage-aware spread evaluation.
		//
		// The old scalar hitchance only asked "does this ray geometrically enter
		// the capsule", which let a point score 1.0 on a leg at long range where
		// the real damage is far below the minimum - the bot then committed to a
		// shot that could not do meaningful damage. These numbers keep the
		// per-direction damage, so selection can reason about *what* a shot hits
		// instead of only *whether* the ray touches the box.
		struct hitchance_metrics
		{
			// Fraction of spread directions whose estimated damage clears
			// min_damage. This is the value compared against the gate.
			float prob_min{};
			// Fraction whose damage is lethal for the target's remaining health.
			float prob_lethal{};
			// Fraction landing on a torso hitbox (chest / stomach / arms).
			float prob_body{};
			// Mean estimated damage across all directions; misses contribute 0.
			float avg_damage{};
			// Fraction that geometrically entered any hitbox. Kept separate so
			// the prefilter can reject obvious misses before paying for damage.
			float geometric{};
		};

		// Inputs shared by the cheap and the full spread evaluation.
		struct spread_eval
		{
			math::vector3 shoot_position{};
			math::vector3 aim_angle{};
			const systems::hitboxes::set* hitboxes{};
			const systems::bones::data* skeleton{};
			int min_damage{};
			int health{};
			// Ratio between the damage a centred trace actually dealt and the
			// unpenetrated baseline. Acts as the penetration factor for the
			// cheap pass, which must not run world traces per direction.
			float center_damage_ratio{ 1.0f };
			bool has_armor{};
			int target_armor{};
			bool has_helmet{};
			int target_team{};
		};

	class lagcomp
	{
	public:
		struct record
			{
				std::uintptr_t pawn{};
				std::uintptr_t game_scene_node{};
				std::uintptr_t bone_cache{};
				int bone_count{};

				systems::bones::data bones[ 128 ]{};
				systems::bones::data bones_backup[ 128 ]{};

				math::vector3 origin{};
				math::vector3 rotation{};

				float simulation_time{};
				int tick{};
				bool valid{};
				bool was_valid{};
				bool is_applied{};
				bool extrapolated{};

				// Resolver state (pitch-exploit detection).
				float next_pitch_realign{};
				bool pitch_exploiting{};
				bool fake_pitch_detected{};

				// CCSPlayerAnimationState snapshot (embedded in CPlayer_MovementServices
				// at +0x310). Captured at record time so the resolver can reconstruct
				// the true body yaw from the smoothed aim yaw instead of brute-forcing.
				bool animstate_valid{};
				std::uint8_t ground_move_state{};
				float prev_aim_yaw{};
				float turn_on_spot_angle{};

				bool setup( std::uintptr_t pawn );
				[[nodiscard]] bool is_valid( ) const;

				void apply( );
				void restore( );
			};

			struct visual_record
			{
				math::vector3 origin{};
				std::array<systems::bones::data, 27> bones{};
			};

			struct extrapolation_data
			{
				math::vector3 origin{};
				math::vector3 velocity{};
				math::vector3 obb_mins{};
				math::vector3 obb_maxs{};
				std::uint32_t flags{};
				float sim_time{};
				float direction{};
			};

			void run( );

			void clear_records( );

			[[nodiscard]] record* get_oldest_valid( std::uintptr_t pawn );
			[[nodiscard]] record* get_oldest_was_valid( std::uintptr_t pawn );
			[[nodiscard]] std::optional<visual_record> get_oldest_was_valid_visual( std::uintptr_t pawn ) const;
			[[nodiscard]] std::vector<record*> get_valid_records( std::uintptr_t pawn );
			[[nodiscard]] std::array<systems::bones::data, 27> get_skeleton( const record& record ) const;

			[[nodiscard]] std::optional<record> extrapolate( std::uintptr_t pawn );
			// Lookahead variant: extrapolates a fixed number of ticks from the
			// newest record instead of the server-relative delta. quiet suppresses
			// the console spam used by the normal extrapolation diagnostics.
			[[nodiscard]] std::optional<record> extrapolate( std::uintptr_t pawn, int ticks, bool quiet = false );

		private:
			void predict_movement( extrapolation_data& data, std::uintptr_t skip_entity ) const;

			std::unordered_map<std::uintptr_t, std::deque<record>> m_records{};
			mutable std::shared_mutex m_records_mtx{};
		};

		class penetration
		{
		public:
			struct weapon_data
			{
				float damage;
				float penetration;
				float range_modifier;
				float range;
				float armor_ratio;
				float headshot_multiplier;
			};

			struct damage_scales
			{
				float ct_head;
				float t_head;
				float ct_body;
				float t_body;
			};

			struct run_context
			{
				std::uintptr_t target_pawn{};
				int target_armor{};
				int target_team{};
				bool has_helmet{};
				damage_scales scales{};
				float armor_ratio{};
				float headshot_multiplier{};
				systems::hitboxes::set hitboxes{};
				lagcomp::record* record{};
			};

			struct result
			{
				float damage{};
				int hitbox{ -1 };
				int hitgroup{ -1 };
				bool penetrated{};
			};

			void prepare( std::uintptr_t weapon_vdata, std::uintptr_t weapon );

			[[nodiscard]] run_context prepare_target( std::uintptr_t target_pawn, lagcomp::record* record ) const;
			[[nodiscard]] bool run( const math::vector3& start, const math::vector3& end, const run_context& ctx, std::uintptr_t local_pawn, int local_team, result& out ) const;
			[[nodiscard]] bool can( const math::vector3& start, const math::vector3& direction, float& out_damage, const systems::local::snapshot& local ) const;
			[[nodiscard]] float get_max_damage( int hitgroup, int target_armor, bool has_helmet, int target_team ) const;
			[[nodiscard]] const weapon_data& get_weapon_data( ) const { return this->m_weapon_data; }

			// Applies the weapon's damage model (hitgroup scaling + armour) to a
			// base damage figure. Exposed so the damage-aware spread evaluation
			// can price each spread direction with the exact same math the
			// penetration trace uses, instead of a second, drifting model.
			[[nodiscard]] float scale_damage_for( int hitgroup, float base_damage, int target_armor, bool has_helmet, int target_team ) const;

			// Current server damage scale convars, sampled fresh so a warmup or
			// mid-round rule change is reflected without a restart.
			[[nodiscard]] damage_scales shared_damage_scales() const;

		private:
			void scale_damage( int hitgroup, int armor, bool has_helmet, int team, float armor_ratio, float headshot_multiplier, const damage_scales& scales, float& damage ) const;
			weapon_data m_weapon_data{};
		};

		class shoot_history
		{
		public:
			struct ring_entry
			{
				int tick{};
				float fraction{};
				math::vector3 position{};
			};

			struct eye_candidate
			{
				math::vector3 position{};
				int player_tick{};
				float player_frac{};
				int lerp_ticks_int{};
				float lerp_ticks_frac{};
				bool is_uninterpolated{};
			};

			struct eye_candidates
			{
				eye_candidate entries[ 2 ]{};
				int count{};
			};

			void snapshot( std::uintptr_t local_pawn, std::uintptr_t weapon_services );
			[[nodiscard]] eye_candidates get_candidates( ) const;
			[[nodiscard]] bool has_data( ) const { return this->m_count > 0; }
			[[nodiscard]] int client_tick( ) const { return this->m_client_tick; }
			[[nodiscard]] float client_tick_frac( ) const { return this->m_client_tick_frac; }
			[[nodiscard]] int server_tick( ) const { return this->m_server_tick; }
			[[nodiscard]] int lerp_ticks_int( ) const { return this->m_lerp_ticks_int; }
			[[nodiscard]] float lerp_ticks_frac( ) const { return this->m_lerp_ticks_frac; }
			[[nodiscard]] int count( ) const { return this->m_count; }
			[[nodiscard]] int oldest_tick( ) const { return this->m_count > 0 ? this->m_entries[ 0 ].tick : -1; }
			[[nodiscard]] int newest_tick( ) const { return this->m_count > 0 ? this->m_entries[ this->m_count - 1 ].tick : -1; }

		private:
			ring_entry m_entries[ 32 ]{};
			int m_count{};
			int m_client_tick{};
			float m_client_tick_frac{};
			int m_server_tick{};
			int m_lerp_ticks_int{};
			float m_lerp_ticks_frac{};
		};

		struct context
		{
			std::uintptr_t weapon{};
			std::uintptr_t weapon_services{};
			std::uintptr_t weapon_vdata{};
			std::uint32_t weapon_type{};
			std::uint16_t item_def_idx{};
			int num_bullets{};
			float recoil_index{};
			int current_tick{};
			int ticks_since_land{};
			float current_time{};
			float weapon_max_speed{};
			float range{};
			bool is_jump_scouting{};
			bool is_scoped{};
			bool valid{};

			float inaccuracy{};
			float spread{};
		};

		void update( );
		void invalidate_if_needed( );

		[[nodiscard]] context& ctx( ) { return this->m_ctx; }
		[[nodiscard]] penetration& pen( ) { return this->m_pen; }
		[[nodiscard]] lagcomp& lc( ) { return this->m_lc; }
		[[nodiscard]] shoot_history& sh( ) { return this->m_sh; }

		[[nodiscard]] bool autowalling( ) const { return this->m_autowalling; }
		[[nodiscard]] lagcomp::record* current_autowall_record( ) const { return this->m_current_autowall_record; }

		[[nodiscard]] int& last_shoot_tick( ) { return this->m_last_shoot_tick; }

		[[nodiscard]] std::uint32_t get_spread_seed( const math::vector3& angles, int tick ) const;
		[[nodiscard]] math::vector2 calculate_spread( int seed, float accuracy, float spread, float recoil_index, int item_def_idx, int num_bullets ) const;
		[[nodiscard]] math::vector3 get_aim_punch( std::uintptr_t local_pawn ) const;
		[[nodiscard]] float calculate_hitchance( const math::vector3& shoot_position, const math::vector3& aim_angle, const systems::hitboxes::entry& hitbox, const systems::bones::data& bone, float inaccuracy, float spread, int samples = 256 ) const;

		// Damage-aware evaluation of a point across the weapon's whole spread
		// cone. `full` picks between the two passes:
		//   false - geometry per direction plus an arithmetic damage estimate
		//           scaled by spread_eval::center_damage_ratio. No world traces,
		//           so it is cheap enough to run on every candidate.
		//   true  - a real penetration trace per direction, so the damage is
		//           exact. Reserved for the single selected point.
		// Both return the same five numbers so they are directly comparable.
		[[nodiscard]] hitchance_metrics evaluate_spread( const spread_eval& eval, float inaccuracy, float spread, bool full, int samples, std::uintptr_t local_pawn, int local_team ) const;

		// Estimated damage of a single spread direction landing on `hitgroup`,
		// before penetration. Mirrors the engine model used by penetration::
		// scale_damage plus the exponential range falloff.
		[[nodiscard]] float estimate_damage( int hitgroup, float distance, float center_damage_ratio, int target_armor, bool has_helmet, int target_team ) const;

		// Distance at which a hitgroup's damage decays to the minimum, squared.
		// Used to skip far points before paying for any spread evaluation.
		[[nodiscard]] float group_range_sqr( int hitgroup, int min_damage, int target_team ) const;

		// Lethal damage for a target, reduced by its armour the way the server
		// counts it, so a heavily armoured target is not treated as needing a
		// full-health headshot to kill.
		[[nodiscard]] float lethal_damage( int health, int armor, bool has_helmet ) const;
		[[nodiscard]] math::vector3 find_spread_correction( const math::vector3& aim_angle, int tick, float inaccuracy, float base_spread ) const;
		[[nodiscard]] math::vector3 get_eye_position( std::uintptr_t local_pawn ) const;
		[[nodiscard]] math::vector3 get_shoot_position( ) const;
		[[nodiscard]] math::vector3 get_interpolated_shoot_position( std::uintptr_t local_pawn, bool newest = false ) const;
		[[nodiscard]] int calculate_stop_ticks( const math::vector3& velocity, float max_speed, std::uintptr_t local_pawn ) const;
		[[nodiscard]] float get_spread( ) const;
		[[nodiscard]] float get_inaccuracy( bool update_accuracy_penalty ) const;
		[[nodiscard]] float get_inaccuracy_at_velocity( std::uintptr_t local_pawn, const math::vector3& velocity ) const;
		[[nodiscard]] float get_air_inaccuracy( float vertical_speed, float jump_initial, float jump_apex ) const;
		[[nodiscard]] bool can_shoot( systems::input::usercmd* cmd, std::uintptr_t local_controller, bool check_next_attack = true ) const;
		[[nodiscard]] bool is_max_accuracy( float inaccuracy ) const;
		[[nodiscard]] math::vector3 simulate_aim_punch( int recoil_index ) const;

		bool ray_vs_capsule( const math::vector3& ray_origin, const math::vector3& ray_dir, const math::vector3& capsule_a, const math::vector3& capsule_b, float radius, float& out_fraction ) const;

	private:
		[[nodiscard]] penetration::damage_scales m_damage_scales() const;

		context m_ctx{};
		penetration m_pen{};
		lagcomp m_lc{};
		shoot_history m_sh{};

		// Parallel rage workers must not overwrite each other's trace record.
		inline static thread_local bool m_autowalling{};
		inline static thread_local lagcomp::record* m_current_autowall_record{ nullptr };

		int m_last_shoot_tick{};
	};

	class misc
	{
	private:
		class antiaim
		{
		public:
			void on_create_move( systems::input::usercmd* cmd );
			void on_render( xdraw::draw_list& draw_list ) const;

			[[nodiscard]] bool has_modified_angles( ) const { return this->m_should_correct || this->m_modified_angles.y != this->m_old_angles.y; }
			[[nodiscard]] const math::vector3& get_modified_angles( ) const { return this->m_modified_angles; }

			// True while antiaim is actively rewriting this command's angles
			// with a non-trivial yaw/pitch change - i.e. movement had to be
			// re-based onto the fake viewangles. The input alignment delta
			// must be suppressed in that case: it would otherwise inject the
			// whole rotation difference as an extra movement impulse.
			[[nodiscard]] bool is_rotating_command( ) const {
				return this->m_antiaim_active
					&& ( std::abs( this->m_intent_angles.x - this->m_modified_angles.x ) >= 0.001f
						|| std::abs( this->m_intent_angles.y - this->m_modified_angles.y ) >= 0.001f );
			}

			// Re-expresses raw movement impulses (real-camera basis) in the
			// basis the current command will be processed with, so features
			// that seed subtick deltas from them stay consistent.
			[[nodiscard]] math::vector3 reproject_moves( const math::vector3& moves ) const;

			// Re-bases this command's movement from the antiaim basis onto a new
			// view basis. The ragebot runs after antiaim and overwrites the
			// command angles with the real aim angle; without this the movement
			// values stay expressed in the (now stale) fake basis and the engine
			// reads WASD against the aim direction, which visibly snaps the path
			// on every shot. No-op when antiaim did not rotate this command.
			void rebasis_movement( systems::input::usercmd* cmd, const math::vector3& new_basis ) const;

		private:
			[[nodiscard]] float get_pitch( float view_pitch );
			[[nodiscard]] float get_yaw( const math::vector3& view_angles, const systems::local::snapshot& local );

			// CreateMove runs several times per command (frames within the
			// tick plus prediction replays). Time-varying yaw modes (spin /
			// jitter) would re-rotate already-converted movement on every
			// pass, so the pristine engine input is snapshotted once per
			// command and every pass re-projects from it - keeping the fix
			// idempotent no matter how often the pipeline reruns.
			void capture_input_snapshot( systems::input::usercmd* cmd );
			void restore_input_snapshot( systems::input::usercmd* cmd );
			void movement_fix( systems::input::usercmd* cmd, const math::vector3& source_angles, const math::vector3& target_angles ) const;
			[[nodiscard]] bool is_near_ladder( std::uintptr_t local_pawn ) const;

			math::vector3 m_old_angles{};
			math::vector3 m_modified_angles{};

			int m_yaw_side{};
			bool m_should_correct{};
			bool m_antiaim_active{};
			float m_indicator_yaw{};

			const systems::input::usercmd* m_intent_cmd{};
			std::int32_t m_intent_command_number{ -1 };
			math::vector3 m_intent_angles{};
			float m_intent_forwardmove{};
			float m_intent_leftmove{};
			std::vector<proto::subtick_move_step> m_intent_subticks{};
		};

		class duckpeek
		{
		public:
			void on_create_move( systems::input::usercmd* cmd );
			void on_override_view( std::uintptr_t view_setup );

		private:
			bool m_was_active{};
			bool m_fake_stand_active{};
		};

		class quickpeek
		{
		public:
			void on_create_move( systems::input::usercmd* cmd );
			void reset_if_needed( );

		private:
			static constexpr std::uint32_t invalid_effect_index{ static_cast<std::uint32_t>( -1 ) };

			void create_particle( );
			void update_particle( );
			void release_particle( );
			void reset( );

			math::vector3 m_saved_origin{};
			bool m_should_retrack{};
			bool m_fired{};
			bool m_active{};
			std::uint32_t m_particle_effect{ invalid_effect_index };
			bool m_particle_loaded{};
			std::uintptr_t m_prev_movement_bits{};
		};

		class autostop
		{
		public:
			void on_create_move( systems::input::usercmd* cmd );

		private:
			[[nodiscard]] float get_effective_accel_base( std::uintptr_t local_pawn, std::uintptr_t movement_services, std::uint32_t flags, float max_weapon_speed ) const;
		};

		antiaim m_antiaim{};
		duckpeek m_duckpeek{};
		quickpeek m_quickpeek{};
		autostop m_autostop{};

	public:
		[[nodiscard]] antiaim& antiaim( ) { return this->m_antiaim; }
		[[nodiscard]] duckpeek& duckpeek( ) { return this->m_duckpeek; }
		[[nodiscard]] quickpeek& quickpeek( ) { return this->m_quickpeek; }
		[[nodiscard]] autostop& autostop( ) { return this->m_autostop; }
	};

	class rage
	{
	public:
		void on_create_move( systems::input::usercmd* cmd );
		void on_render( xdraw::draw_list& draw_list );

		[[nodiscard]] bool should_stop( ) const noexcept { return this->m_should_stop; }
		[[nodiscard]] bool is_firing_this_tick( ) const noexcept { return this->m_firing_this_tick; }
		[[nodiscard]] bool should_stop_between_shots( ) const noexcept;
		[[nodiscard]] bool is_cocking_revolver( ) const noexcept { return this->m_revolver_cock_ticks > 0; }
		[[nodiscard]] bool should_release_duck_for_shot( ) const noexcept { return this->m_release_duck_for_shot; }
		[[nodiscard]] bool duckpeek_wants_reduck( ) const noexcept { return this->m_duckpeek_reduck; }
		void clear_duckpeek_reduck( ) noexcept { this->m_duckpeek_reduck = false; }

		static constexpr auto k_max_lagcomp_records{ 16 };
		// Records are spread evenly across the valid backtrack window so poses
		// between the newest and oldest are evaluated too. The scan loop still
		// stops early when a guaranteed direct kill is found on a newer pose.
		static constexpr auto k_max_scan_records{ 6 };

		// Rewrites the current command's input history so the shot resolves on
		// the earliest legal weapon tick. Called from create_move after the
		// entire combat pipeline so doubletap owns the final tick shifting.
		void run_double_tap( systems::input::usercmd* cmd );

	private:
		struct aim_context
		{
			math::vector3 view_angles{};
			math::vector3 velocity{};

			float predicted_inaccuracy{};
			float spread{};

			float weapon_max_speed{};
			float accurate_threshold{};
			bool on_ground{};
			bool is_scoped{};
		};

		struct stop_prediction
		{
			math::vector3 eye{};
			float inaccuracy{};
		};

		struct candidate
		{
			std::uintptr_t pawn{};
			int health{};
			int armor{};
			float min_damage{};
			float priority{};
			// Movement speed (units/s) between the two newest poses; used to
			// shrink the backtrack window for fast-moving (peeking) targets.
			float speed{};
			std::array<shared::lagcomp::record*, k_max_lagcomp_records> records{};
			int record_count{};
		};

		struct scan_hit
		{
			math::vector3 position{};
			math::vector3 aim_angle{};
			float damage{};
			float score{};
			float fov{};
			int hitbox_index{};
			int hitgroup{};
			int bone_index{};
			systems::hitboxes::entry hitbox{};
			bool is_center{};
			bool penetrated{};
			bool meets_min_damage{ true };
			bool is_backstab{};
			// Came from a speculative resolver head-pose guess rather than a
			// measured hitbox. Purely additive: may upgrade a shot, never block one.
			bool is_resolver_guess{};
			int attack_type{};
			float priority{};
			shared::shoot_history::eye_candidate source_eye{};

			std::uintptr_t pawn{};
			int health{};
			shared::lagcomp::record* record{};
		};

		struct target
		{
			scan_hit hit{};
			float hitchance{};
			float score{};
			bool valid{};

			[[nodiscard]] bool is_lethal( ) const noexcept
			{
				return this->hit.damage >= static_cast< float >( this->hit.health );
			}
		};

		struct knife_info
		{
			bool can_slash{};
			bool can_stab{};
			bool charged{};
			float armor_ratio{};
		};

		[[nodiscard]] aim_context build_context( systems::input::usercmd* cmd, const systems::local::snapshot& local ) const;
		[[nodiscard]] std::optional<stop_prediction> predict_stop( const aim_context& ctx, const math::vector3& current_eye, const systems::local::snapshot& local ) const;
		[[nodiscard]] std::vector<candidate> gather_candidates( const systems::local::snapshot& local, float max_distance_sq = 0.0f ) const;

		void run_gun( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local, bool allow_fire = true );
		void run_taser( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local );
		void run_knife( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local );
		void auto_revolver( systems::input::usercmd* cmd, const aim_context& ctx, const systems::local::snapshot& local );

		[[nodiscard]] std::vector<scan_hit> scan_players( const math::vector3& eye, float inaccuracy, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local ) const;
		[[nodiscard]] std::vector<scan_hit> scan_player( const math::vector3& eye, float inaccuracy, const aim_context& ctx, candidate& cand, shared::lagcomp::record* record, const systems::local::snapshot& local ) const;
		[[nodiscard]] target select_best( const aim_context& aim_ctx, const std::vector<scan_hit>& hits, float eval_inaccuracy ) const;
		[[nodiscard]] float evaluate_hitchance( const scan_hit& hit, const aim_context& ctx, float inaccuracy ) const;
		[[nodiscard]] float get_standing_inaccuracy( const systems::local::snapshot& local, const aim_context& ctx ) const;

		[[nodiscard]] std::vector<scan_hit> scan_taser( const math::vector3& eye, const aim_context& ctx, std::vector<candidate>& candidates, const systems::local::snapshot& local ) const;

		[[nodiscard]] knife_info get_knife_info( const systems::local::snapshot& local ) const;
		[[nodiscard]] std::vector<scan_hit> scan_knife( const math::vector3& eye, const aim_context& ctx, const knife_info& info, std::vector<candidate>& candidates, const systems::local::snapshot& local ) const;

		void fire_gun( systems::input::usercmd* cmd, target& tgt, bool was_forced, const math::vector3& shoot_eye, const systems::local::snapshot& local, bool subtick_attack );
		void fire_melee( systems::input::usercmd* cmd, const target& tgt, const systems::local::snapshot& local );

		void apply_autoscope( systems::input::usercmd* cmd, const systems::local::snapshot& local ) const;

		[[nodiscard]] std::vector<math::vector3> generate_multipoints( const systems::hitboxes::entry& hitbox, const math::vector3& center, const math::quaternion& bone_rot, float pointscale, const math::vector3& shoot_pos, float inaccuracy ) const;
		[[nodiscard]] bool should_stop_movement( const aim_context& ctx ) const;
		[[nodiscard]] float get_min_damage( const settings::combat::ragebot::weapon_group& config, int target_health, bool override_active ) const;
		[[nodiscard]] float get_knife_damage( float raw, int armor, float armor_ratio ) const;
		[[nodiscard]] systems::tracing::result trace_taser_hit( const math::vector3& origin, const math::vector3& forward, float range, std::uintptr_t target_pawn, std::uintptr_t local_pawn ) const;
		[[nodiscard]] systems::tracing::result trace_knife_hit( const math::vector3& origin, const math::vector3& forward, float reach, std::uintptr_t target_pawn, std::uintptr_t local_pawn ) const;

		enum class penetration_crosshair_state : std::uint8_t
		{
			unavailable,
			blocked,
			penetrable
		};

		void update_penetration_crosshair( const systems::local::snapshot& local );
		void draw_penetration_crosshair( xdraw::draw_list& draw_list ) const;

		[[nodiscard]] bool process_doubletap( systems::input::usercmd* cmd, const systems::local::snapshot& local, bool charge_dt );

		void update_dt_charge( const systems::local::snapshot& local );
		[[nodiscard]] float get_dt_charge_progress( ) const;
		[[nodiscard]] bool is_dt_ready( ) const { return this->m_dt_ready; }

		void apply_rapid_fire( systems::input::usercmd* cmd );

		struct timing_result
		{
			bool should_wait{};
		};

		[[nodiscard]] timing_result evaluate_shot_timing( const aim_context& ctx, std::vector<candidate>& candidates, const target& best, const systems::local::snapshot& local ) const;
		void update_miss_tracking( );

		[[nodiscard]] float get_target_priority( std::uintptr_t pawn, const math::vector3& local_eye, const systems::local::snapshot& local ) const;
		[[nodiscard]] bool is_enemy_aiming_at_us( std::uintptr_t pawn, const math::vector3& local_eye ) const;
		[[nodiscard]] bool safe_line_clear( const math::vector3& eye, const target& tgt, const systems::local::snapshot& local ) const;

		void draw_doubletap_indicator( xdraw::draw_list& draw_list ) const;

		bool m_should_stop{};
		bool m_firing_this_tick{};
		bool m_release_duck_for_shot{};
		bool m_duckpeek_reduck{};

		std::uint32_t m_last_shot_tick{};

		std::uint8_t m_knife_attack{};
		bool m_zeus_fired{};

		int m_revolver_cock_ticks{};
		std::atomic<penetration_crosshair_state> m_penetration_crosshair_state{ penetration_crosshair_state::unavailable };

		std::vector<shared::lagcomp::record> m_extrapolated_records{};

		// Shot timing (ideal tick) hold state.
		std::uintptr_t m_hold_pawn{};
		int m_hold_ticks{};

		// Adaptive hitchance / miss tracking.
		float m_adaptive_hc_boost{};
		int m_miss_streak{};
		std::uintptr_t m_last_shot_pawn{};
		int m_last_shot_health{};
		int m_last_shot_tick_when{};

		// Doubletap charge state.
		int m_dt_charge_start{ -1 };
		bool m_dt_ready{};
		int m_dt_shot_count{};

		// Rotating multipoint state (vendetta-style)
		float m_head_angle{ 0.f };
		float m_other_z_step{ 0.f };
		static constexpr int k_head_steps{ 48 };
		static constexpr int k_other_z_steps{ 48 };

		mutable std::vector<shared::lagcomp::record> m_timing_records{};

		struct debug_point
		{
			math::vector3 position{};
			int hitbox_index{};
			bool is_center{};
		};

		mutable std::vector<debug_point> m_debug_points{};
		mutable std::mutex m_debug_mtx{};
	};

	class legit
	{
	public:
		void on_create_move( systems::input::usercmd* cmd );
		void on_render( xdraw::draw_list& draw_list );
		void invalidate_if_needed( );

		[[nodiscard]] bool has_target( ) const noexcept { return this->m_target.has_target( ); }
		[[nodiscard]] bool should_stop( ) const noexcept { return this->m_should_stop; }

	private:
		struct scan_point
		{
			math::vector3 position{};
			float damage{};
			float fov{};
			int hitgroup{};
			std::size_t cfg_index{};
			int bone_index{};
			systems::hitboxes::entry hitbox{};
			bool visible{};
			bool is_center{};
			bool valid{};
		};

		struct target_result
		{
			std::uintptr_t pawn{};
			scan_point best_point{};
			math::vector3 aim_angle{};
			float hitchance{};
			float score{};
			float fov{};
			int health{};
			shared::lagcomp::record* record{};

			[[nodiscard]] bool has_target( ) const noexcept { return this->best_point.valid; }
		};

		[[nodiscard]] target_result find_target( const math::vector3& shoot_position, const math::vector3& view_angles, const settings::combat::legitbot::weapon_group& config, const systems::local::snapshot& local ) const;
		[[nodiscard]] scan_point scan_player( std::uintptr_t pawn, shared::lagcomp::record* record, const math::vector3& shoot_position, const math::vector3& view_angles, const settings::combat::legitbot::weapon_group& config, const systems::local::snapshot& local ) const;

		void apply_aimbot( systems::input::usercmd* cmd, const target_result& tgt, const math::vector3& view_angles, const math::vector3& aim_punch, const settings::combat::legitbot::weapon_group& config, const systems::local::snapshot& local );
		bool apply_triggerbot( systems::input::usercmd* cmd, const math::vector3& shoot_position, const math::vector3& view_angles, const math::vector3& aim_punch, const settings::combat::legitbot::weapon_group& config, const systems::local::snapshot& local, bool manual_attack_before_trigger = false );
		void apply_rcs( math::vector3& aim_angle, const math::vector3& aim_punch, int rand_min, int rand_max ) const;

		void update_standalone_rcs( const math::vector3& view_angles, const math::vector3& aim_punch, int amount, int rand_min, int rand_max, bool apply, const systems::local::snapshot& local );
		[[nodiscard]] float compute_rcs_factor( int rand_min, int rand_max ) const;

		void draw_fov( xdraw::draw_list& draw_list, const math::vector3& view_angles, const math::vector3& aim_punch, float fov_degrees, const config::col& color, bool rcs_active ) const;

		[[nodiscard]] static int hitgroup_to_cfg( int hitgroup );

		target_result m_target{};
		math::vector3 m_old_punch{};
		mutable float m_last_significant_punch_time{};

		float m_remainder_x{};
		float m_remainder_y{};

		float m_trigger_delay_start{};
		float m_trigger_release_time{};
		std::uintptr_t m_trigger_pending_pawn{};

		math::vector3 m_cached_view_angles{};
		math::vector3 m_cached_aim_punch{};
		bool m_should_stop{};
	};

	// Always-on input history emitter that fakes interpolated view angles
	// across the command's input_history entries so the engine never sees a
	// hard angle snap from silent-aim / fake angles (rage + legit).
	class vac
	{
	public:
		void on_create_move( systems::input::usercmd* cmd );
		void on_level_init( );

	private:
		[[nodiscard]] float wrap180( float value ) const;

		// Reference angles of the last pass; drives the interpolation. Mirrors
		// FVA's s_reference_bits (starts {0,0,0}, updated to `prior` each tick).
		math::vector3 m_reference{};
	};

} // namespace features::combat
