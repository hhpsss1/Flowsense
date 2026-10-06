#pragma once

#include <string>
#include <vector>

namespace rendering {

	class context
	{
	public:
		bool initialize( IDXGISwapChain* swap_chain );
		void shutdown( );
		void on_present( IDXGISwapChain* swap_chain );
		void on_resize_buffers( );
		void on_resize_buffers_post( IDXGISwapChain* swap_chain );

		[[nodiscard]] HWND get_window( ) const { return this->m_window; }
		[[nodiscard]] ID3D11Device* get_device( ) const { return this->m_device; }
		[[nodiscard]] ID3D11DeviceContext* get_context( ) const { return this->m_context; }
		[[nodiscard]] bool is_initialized( ) const { return this->m_initialized; }
		[[nodiscard]] bool ui_assets_ready( ) const { return this->m_ui_assets_ready; }
        ID3D11RenderTargetView* get_rtv( ) const { return this->m_rtv; }

	private:
		void create_rtv( IDXGISwapChain* swap_chain );
		void setup_zdraw( HWND window );
		bool try_bind_ui_assets( );

		ID3D11Device* m_device{ nullptr };
		ID3D11DeviceContext* m_context{ nullptr };
		ID3D11RenderTargetView* m_rtv{ nullptr };
		HWND m_window{ nullptr };
		bool m_initialized{ false };
		bool m_ui_assets_ready{ false };
	};

    class menu
    {
    public:
        void initialize_graphics( );

        void draw( );
        void shutdown( );

        void toggle( ) { this->m_open = !this->m_open; }
		[[nodiscard]] bool is_open( ) const { return this->m_open; }
		[[nodiscard]] float pos_x( ) const { return this->m_x; }
		[[nodiscard]] float pos_y( ) const { return this->m_y; }
		void apply_saved_cursor( );
		void handle_open_state( );

        void sync_theme_style( ) const;

        enum class tab : int
        {
            ragebot, player, world, skins, misc, config, lua, count
        };

    private:
		bool draw_intro( );
        void draw_header( );
        void draw_side_bar( float h );
        void draw_top_bar( float w );
        void try_load_user_avatar( );

        void draw_ragebot( float group_w ) const;
        void draw_player( float group_w ) const;
        void draw_world( float group_w ) const;
        void draw_skins( float group_w ) const;
        void draw_misc( float group_w ) const;
        void draw_config( float group_w );
        void draw_lua( float group_w ) const;
        void handle_resize( );

        bool m_open{ true };
        bool m_config_modal_open{};
        bool m_config_cloud_refresh_pending{};
        bool m_config_advanced_open{};
        bool m_last_open{ true };
        float m_open_anim{ 1.0f };
        std::uint8_t m_saved_relative_mouse{};
		bool m_has_saved_cursor{};
		int m_saved_cursor_x{};
		int m_saved_cursor_y{};

        float m_x{ 100.0f };
        float m_y{ 60.0f };
		float m_w{ 620.0f };
		float m_h{ 455.0f };

        bool m_resizing{};
        float m_resize_start_x{};
        float m_resize_start_y{};
        float m_resize_start_w{};
        float m_resize_start_h{};
        float m_body_x{};
        float m_body_y{};
        float m_body_w{};
        float m_body_h{};

        float m_content_x{};
        float m_content_y{};
        float m_content_w{};
        float m_content_h{};
        float m_col_w{};

        int m_tab{};
        int m_subtab{};
        int m_subtab_pill_tab{ -1 };
        float m_subtab_pill_x{ -1.0f };
		int m_weapon_content_tab{ -1 };
		int m_weapon_content_subtab{ -1 };
		int m_weapon_content_direction{ 1 };
		float m_weapon_content_anim{ 1.0f };
		float m_intro_elapsed{};
		float m_intro_assets_ready_at{ -1.0f };
		float m_intro_bar_phase{};
		bool m_intro_base_graphics_ready{};
		bool m_intro_finished{};
		float m_user_avatar_retry_delay{};

        struct textures
        {
            struct entry
            {
                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource{};
                int width{};
                int height{};
            };

            entry brand{};
            entry user{};
            entry tabs[ 7 ]{};
            entry cfg_folder_on{};
            entry cfg_folder_off{};
            entry cfg_cloud_on{};
            entry cfg_cloud_off{};
            entry cfg_plus{};
			entry intro_splash{};
        } m_textures{};

        static constexpr auto k_max_subtabs{ 7 };

        struct subtab_info
        {
            const char* names[ k_max_subtabs ]{};
            int count{};
        };

        static constexpr subtab_info k_subtab_defs[ static_cast< int >( tab::count ) ]
        {
            { { "Pistol", "Smg", "Rifle", "Shotgun", "Sniper", "Lmg" }, 6 },
            { { "Enemies", "Teammates", "Local" },                   3 },
            { { "Esp", "Weather" },                                     2 },
            { { "Weapons", "Knives", "Gloves", "Agents" },              4 },
            { { "General", "View" },                                    2 },
            { { "General", "Settings" },                                2 },
            { { "General" },                                            1 }
        };
    };

	class widgets
	{
	public:
		void draw( );

		static inline std::string s_map_name{};

	private:
		void watermark( xdraw::draw_list& draw_list );
		void indicators( xdraw::draw_list& draw_list );
		void manual_aa( xdraw::draw_list& draw_list );
		void keybinds( xdraw::draw_list& draw_list );
	};

	class fonts
	{
	public:
		enum class size : std::uint8_t
		{
			petite,
			normal,
			big,
			count
		};

		struct family_t
		{
			std::array<xdraw::font*, static_cast< std::size_t >( size::count )> sizes{ };

			xdraw::font* operator[]( size size ) const { return this->sizes[ static_cast< std::size_t >( size ) ]; }
			xdraw::font*& operator[]( size size ) { return this->sizes[ static_cast< std::size_t >( size ) ]; }
		};

		void initialize( );

		family_t inter_medium{};
		family_t inter_bold{};
		family_t nunito_regular{};
		family_t inter_tight{};

	private:
		void load_family( family_t& family, std::span<const std::byte> data, const std::array<float, static_cast< std::size_t >( size::count )>& sizes );
	};

	inline context g_context{};
	inline menu g_menu{};
	inline widgets g_widgets{};
	inline fonts g_fonts{};

}
