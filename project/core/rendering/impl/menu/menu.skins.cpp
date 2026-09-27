#include <pch/pch.hpp>
#include <charconv>
#include <core/features/features.hpp>
#include <core/settings.hpp>

#include "CModelChanger.hpp"

#include "../../rendering.hpp"

namespace rendering {

	namespace detail {

		// RAII guard – hold as a named local, never as a temporary.
		// Two separate skin_map() temporaries were the crash: each locks the
		// mutex for one call and drops it, leaving the iterator from the first
		// dangling by the time .end() is called on the second.
		struct skin_map_guard
		{
			std::lock_guard<std::recursive_mutex> lock;
			std::unordered_map<std::int16_t, settings::changer::applied_skin>& m;

			skin_map_guard( ) : lock{ settings::g_changer.skins.mx }, m{ settings::g_changer.skins.data } { }

			auto find( std::int16_t key )  -> decltype( m.find( key ) )  { return m.find( key ); }
			auto end( )                    -> decltype( m.end( ) )       { return m.end( ); }
			auto& operator[]( std::int16_t key )                         { return m[ key ]; }
			void erase( std::int16_t key )                               { m.erase( key ); }
		};

		// Helper wrappers for call sites that only need read-copy or simple write.
		[[nodiscard]] inline static std::optional<settings::changer::applied_skin> get_applied_skin( std::int16_t def_index )
		{
			skin_map_guard g;
			const auto it = g.find( def_index );
			if ( it == g.end( ) )
				return std::nullopt;
			return settings::changer::snapshot_skin( it->second );
		}

		template <typename F>
		inline static bool update_applied_skin( std::int16_t def_index, F&& fn )
		{
			skin_map_guard g;
			const auto it = g.find( def_index );
			if ( it != g.end( ) )
			{
				fn( it->second );
				return true;
			}
			return false;
		}

		template <typename F>
		inline static void mutate_applied_skin( std::int16_t def_index, F&& fn )
		{
			skin_map_guard g;
			fn( g[ def_index ] );
		}

		inline static void erase_applied_skin( std::int16_t def_index )
		{
			skin_map_guard g;
			g.erase( def_index );
		}

		// Stable storage for the paint color pickers. xui's color_picker keeps
		// a raw pointer to the bound color for as long as its popup is open, so
		// handing it a live skins.data node means writing into freed heap once
		// the map is rehashed (e.g. by an "apply" that erases entries). The
		// pickers bind to this buffer instead and the overlay syncs it to the
		// map under the map mutex every frame.
		//
		// The buffer is pre-reserved so inserting a kit never rehashes it: an
		// open popup's m_col points into a node, and nodes must stay put for the
		// popup's whole lifetime or the next drag writes into freed heap.
		inline std::unordered_map<std::int16_t, std::array<xdraw::color, 4>> g_paint_color_buf{ [] {
			std::unordered_map<std::int16_t, std::array<xdraw::color, 4>> m;
			m.reserve( 512 );
			return m;
		}( ) };

		enum class skins_page : int
		{
			grid,
			browser
};

		struct skins_state
		{
			skins_page current{ skins_page::grid };
			skins_page target{ skins_page::grid };
			float fade{ 1.0f };

			std::int16_t browsing_def{};
			int browsing_agent_team{};
			std::string search_buf{};
		};

		skins_state skins_ui{};

		constexpr xdraw::color k_rarity_colors[ 8 ]
		{
			xdraw::color{ 235, 235, 235, 255 },
			xdraw::color{ 138, 173, 233, 255 },
			xdraw::color{  77, 116, 196, 255 },
			xdraw::color{ 138,  86, 207, 255 },
			xdraw::color{ 211,  44, 230, 255 },
			xdraw::color{ 235,  75,  75, 255 },
			xdraw::color{ 228, 174,  57, 255 },
			xdraw::color{ 255, 215,   0, 255 }
		};

		constexpr auto k_card_w_ref{ 118.0f };
		constexpr auto k_card_h_ref{ 124.0f };
		constexpr auto k_card_gap{ 8.0f };
		constexpr auto k_columns{ 5 };
		constexpr auto k_image_h_ratio{ 0.62f };
		constexpr auto k_rarity_bar_h{ 2.0f };

		static inline const char* wear_tier( float w )
		{
			if ( w <= 0.07f )
			{
				return "FN";
			}

			if ( w <= 0.15f )
			{
				return "MW";
			}

			if ( w <= 0.38f )
			{
				return "FT";
			}

			if ( w <= 0.45f )
			{
				return "WW";
			}

			return "BS";
		}

		static inline std::string format_wear( float w )
		{
			char buf[ 32 ]{};
			std::snprintf( buf, sizeof( buf ), "%.4f %s", w, wear_tier( w ) );
			return std::string{ buf };
		}

		static inline void request_page( skins_page p, std::int16_t def = 0 )
		{
			if ( skins_ui.current == p && skins_ui.target == p )
			{
				return;
			}

			skins_ui.target = p;
			skins_ui.fade = 1.0f;

			if ( p == skins_page::browser )
			{
				skins_ui.browsing_def = def;
			}
			else
			{
				skins_ui.browsing_agent_team = 0;
			}
		}

		static inline const std::vector<const features::changer::econ_item_system::item_def*>& select_weapons( int subtab )
		{
			auto& econ = features::changer::g_econ_item_system;

			switch ( subtab )
			{
			case 1: return econ.knives( );
			case 2: return econ.gloves( );
			case 3: return econ.agents( );
			default: return econ.guns( );
			}
		}

		// Runtime item-def entries that take up a slot but can't be skinned -
		// the schema's placeholder agents.
		static inline bool is_hidden_def( const features::changer::econ_item_system::item_def* def )
		{
			if ( !def )
			{
				return true;
			}

			const std::string_view name{ def->localized_name };
			return name == "Default CT Agent" || name == "Default T Agent";
		}

		class wear_sub_overlay : public xui::overlay
		{
		public:
			wear_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def } {}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##wear_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				auto _smg0 = skin_map_guard{}; const auto it = _smg0.find( this->m_def );
				if ( it != _smg0.end( ) )
				{
					xui::slider_float( "Wear", it->second.wear, 0.0f, 1.0f, "%.4f" );
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 220.0f, 56.0f };
			}

			std::int16_t m_def{};
			float m_open_anim{};
		};

		class seed_sub_overlay : public xui::overlay
		{
		public:
			seed_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def }
			{
				auto _smg1 = skin_map_guard{}; const auto it = _smg1.find( def );
				if ( it != _smg1.end( ) )
				{
					this->m_buf = std::to_string( it->second.seed );
				}
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##seed_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				if ( xui::text_input( "##seed_input", this->m_buf, 4, "0-1000" ) )
				{
					auto _smg2 = skin_map_guard{}; const auto it = _smg2.find( this->m_def );
					if ( it != _smg2.end( ) )
					{
						int v = 0;
						if ( !this->m_buf.empty( ) )
						{
							const char* first = this->m_buf.data( );
							const char* last = first + this->m_buf.size( );
							auto [ptr, ec] = std::from_chars( first, last, v );
							if ( ec != std::errc( ) )
							{
								v = 0;
							}
						}

						it->second.seed = std::clamp( v, 0, 1000 );
					}
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 180.0f, 50.0f };
			}

			std::int16_t m_def{};
			std::string m_buf{};
			float m_open_anim{};
		};

		class nametag_sub_overlay : public xui::overlay
		{		public:
			nametag_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def }
			{
				auto _smg3 = skin_map_guard{}; const auto it = _smg3.find( def );
				if ( it != _smg3.end( ) )
				{
					this->m_buf = settings::changer::valid_nametag( it->second.nametag ) ? it->second.nametag : std::string{};
				}
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##nametag_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				if ( xui::text_input( "##nametag_input", this->m_buf, 20, "Nametag..." ) )
				{
					auto _smg4 = skin_map_guard{}; const auto it = _smg4.find( this->m_def );
					if ( it != _smg4.end( ) )
					{
						it->second.nametag = this->m_buf;
					}
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 180.0f, 50.0f };
			}

			std::int16_t m_def{};
			std::string m_buf{};
			float m_open_anim{};
		};

		class stattrak_value_sub_overlay : public xui::overlay
		{
		public:
			stattrak_value_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def }
			{
				auto _smg5 = skin_map_guard{}; const auto it = _smg5.find( def );
				if ( it != _smg5.end( ) )
				{
					this->m_buf = std::to_string( it->second.stattrak_value );
				}
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##stattrak_val_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				if ( xui::text_input( "##stattrak_val_input", this->m_buf, 7, "0-999999" ) )
				{
					auto _smg6 = skin_map_guard{}; const auto it = _smg6.find( this->m_def );
					if ( it != _smg6.end( ) )
					{
						int v = 0;
						if ( !this->m_buf.empty( ) )
						{
							const char* first = this->m_buf.data( );
							const char* last = first + this->m_buf.size( );
							auto [ptr, ec] = std::from_chars( first, last, v );
							if ( ec != std::errc( ) )
							{
								v = 0;
							}
						}

						it->second.stattrak_value = std::clamp( v, 0, 999999 );
					}
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 180.0f, 50.0f };
			}

			std::int16_t m_def{};
			std::string m_buf{};
			float m_open_anim{};
		};

		class paint_color_sub_overlay : public xui::overlay
		{
		public:
			paint_color_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def ) : overlay{ id, anchor }, m_def{ def }
			{
				this->m_enable = xui::setting{ {}, {}, "paint color", "skins" };
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##paint_color_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				auto& color_buf = g_paint_color_buf[ this->m_def ];
				bool entry_exists = false;

				{
					std::lock_guard<std::recursive_mutex> lock{ settings::g_changer.skins.mx };

					const auto it = settings::g_changer.skins.data.find( this->m_def );
					if ( it != settings::g_changer.skins.data.end( ) )
					{
						entry_exists = true;
						// Always read the toggle flag (plain bool, not a drag value)
						this->m_enable.value = it->second.paint_color;
						// Sync color_buf from the map only once on first open.
						// If we reset it every frame we clobber an in-progress drag.
						if ( !this->m_initialized )
						{
							this->m_initialized = true;
							for ( auto i = 0; i < 4; ++i )
							{
								color_buf[ i ] = it->second.paint_colors[ i ];
							}
						}
					}
				}

				if ( entry_exists )
				{
					xui::checkbox( "Custom paint color", this->m_enable );

					for ( auto i = 0; i < 4; ++i )
					{
						char label[ 24 ]{};
						std::snprintf( label, sizeof( label ), "g_vColor%d", i );
						xui::color_picker( label, color_buf[ i ], 0.0f, false );
					}

					// Always flush picker state back so every drag frame persists
					{
						std::lock_guard<std::recursive_mutex> lock{ settings::g_changer.skins.mx };

						const auto it = settings::g_changer.skins.data.find( this->m_def );
						if ( it != settings::g_changer.skins.data.end( ) )
						{
							it->second.paint_color = this->m_enable.value;
							for ( auto i = 0; i < 4; ++i )
							{
								it->second.paint_colors[ i ] = color_buf[ i ];
							}
						}
					}
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 260.0f, 210.0f };
			}

			std::int16_t m_def{};
			xui::setting m_enable{};
			float m_open_anim{};
			bool m_initialized{ false };
		};

		class sticker_keychain_sub_overlay : public xui::overlay
		{
		public:
			sticker_keychain_sub_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def, bool keychain_mode ) : overlay{ id, anchor }, m_def{ def }, m_keychain_mode{ keychain_mode }
			{
				auto _smg7 = skin_map_guard{}; const auto it = _smg7.find( def );
				if ( it != _smg7.end( ) )
				{
					this->m_seed_buf = std::to_string( it->second.keychain.seed );
				}
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				const auto popup = this->get_popup( );
				if ( input.mouse_clicked && !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;
					return false;
				}

				return popup.contains( input.mouse_x, input.mouse_y );
			}

			void render( const xui::style& style, const xui::input_state& ) override
			{
				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto et = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = et;
				const auto animated_h = popup.h * et;
				const auto pr = style.popup_rounding;

				auto& dl = xdraw::get( xdraw::layer::top );

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				if ( et < 0.2f )
				{
					return;
				}

				xui::draw::push_layer( xdraw::layer::top );
				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				xui::window_state ws{};
				ws.title = "##sticker_keychain_sub";
				ws.bounds = popup;
				ws.cursor_x = style.window_pad_x;
				ws.cursor_y = style.window_pad_y;
				ws.is_child = true;

				auto& c = xui::ctx( );
				c.windows.push_back( std::move( ws ) );
				xui::push_id( this->m_id );

				const auto prev_inside = c.inside_overlay;
				c.inside_overlay = this->m_id;

				xui::text_input( "##pick_search", this->m_search_buf, 32, "search" );

				auto _smg8 = skin_map_guard{}; const auto it = _smg8.find( this->m_def );
				if ( it != _smg8.end( ) )
				{
					auto& skin = it->second;

					if ( !this->m_keychain_mode )
					{
						static constexpr const char* slot_items[ 5 ]
						{
							"Slot 1", "Slot 2", "Slot 3", "Slot 4", "Slot 5"
						};

						const auto slot_changed = xui::combo( "Slot", this->m_slot, slot_items, 5 );
						if ( slot_changed || this->m_slot != this->m_synced_slot )
						{
							this->m_synced_slot = this->m_slot;
							this->m_cache_key = -1;
						}

						auto& st = skin.stickers[ this->m_slot ];

						const auto& list = features::changer::g_econ_item_system.stickers( );
						this->ensure_pick_list( false );
						this->apply_filter( );
						this->sync_pick( list, st.id );

						if ( xui::combo( "Sticker##stk", this->m_pick, this->m_filtered_ptrs.data( ), static_cast< int >( this->m_filtered_ptrs.size( ) ) ) )
						{
							const auto orig = this->m_filtered_idx[ static_cast< std::size_t >( this->m_pick ) ];
							st.id = ( orig > 0 ) ? list[ static_cast< std::size_t >( orig - 1 ) ].id : 0;
							this->m_last_id = st.id;
						}

						if ( st.id != 0 )
						{
							xui::slider_float( "Wear", st.wear, 0.0f, 1.0f, "%.4f" );
							xui::slider_float( "Scale", st.scale, 0.1f, 3.0f, "%.2f" );
							xui::slider_float( "Rotation", st.rotation, 0.0f, 360.0f, "%.1f" );
							xui::slider_float( "Offset X", st.offset_x, -1.0f, 1.0f, "%.2f" );
							xui::slider_float( "Offset Y", st.offset_y, -1.0f, 1.0f, "%.2f" );
						}
					}
					else
					{
						auto& kc = skin.keychain;

						const auto& list = features::changer::g_econ_item_system.keychains( );
						this->ensure_pick_list( true );
						this->apply_filter( );
						this->sync_pick( list, kc.id );

						if ( xui::combo( "Keychain##kc", this->m_pick, this->m_filtered_ptrs.data( ), static_cast< int >( this->m_filtered_ptrs.size( ) ) ) )
						{
							const auto orig = this->m_filtered_idx[ static_cast< std::size_t >( this->m_pick ) ];
							kc.id = ( orig > 0 ) ? list[ static_cast< std::size_t >( orig - 1 ) ].id : 0;
							this->m_last_id = kc.id;
						}

						if ( kc.id != 0 )
						{
							if ( xui::text_input( "##keychain_seed", this->m_seed_buf, 6, "seed" ) )
							{
								int v = 0;
								if ( !this->m_seed_buf.empty( ) )
								{
									const char* first = this->m_seed_buf.data( );
									const char* last = first + this->m_seed_buf.size( );
									auto [ptr, ec] = std::from_chars( first, last, v );
									if ( ec != std::errc( ) )
									{
										v = 0;
									}
								}

								kc.seed = std::clamp( v, 0, 100000 );
							}

							xui::slider_float( "Offset X", kc.offset_x, -1.0f, 1.0f, "%.2f" );
							xui::slider_float( "Offset Y", kc.offset_y, -1.0f, 1.0f, "%.2f" );
							xui::slider_float( "Offset Z", kc.offset_z, -1.0f, 1.0f, "%.2f" );
						}
					}
				}

				c.inside_overlay = prev_inside;

				xui::pop_id( );
				c.windows.pop_back( );
				dl.pop_clip( );
				xui::draw::pop_layer( );
			}

		private:
			void ensure_pick_list( bool keychains )
			{
				const auto& list = keychains
					? features::changer::g_econ_item_system.keychains( )
					: features::changer::g_econ_item_system.stickers( );

				const auto key = static_cast< int >( list.size( ) ) * 2 + ( keychains ? 1 : 0 );
				if ( key == this->m_cache_key )
				{
					return;
				}

				this->m_cache_key = key;

				this->m_names.clear( );
				this->m_names.reserve( list.size( ) + 1 );
				this->m_names.emplace_back( "None" );

				for ( const auto& kit : list )
				{
					this->m_names.emplace_back( kit.localized_name.empty( ) ? kit.name : kit.localized_name );
				}

				this->m_ptrs.clear( );
				this->m_ptrs.reserve( this->m_names.size( ) );
				for ( const auto& n : this->m_names )
				{
					this->m_ptrs.push_back( n.c_str( ) );
				}
			}

			void sync_pick( const std::vector<features::changer::econ_item_system::kit_entry>& list, std::int16_t id )
			{
				if ( !this->m_pick_dirty && id == this->m_last_id )
				{
					return;
				}

				this->m_pick_dirty = false;
				this->m_last_id = id;

				auto orig = 0;
				for ( auto i = 0; i < static_cast< int >( list.size( ) ); ++i )
				{
					if ( list[ static_cast< std::size_t >( i ) ].id == id )
					{
						orig = i + 1;
						break;
					}
				}

				this->m_pick = 0;
				if ( orig > 0 )
				{
					for ( auto r = 0; r < static_cast< int >( this->m_filtered_idx.size( ) ); ++r )
					{
						if ( this->m_filtered_idx[ static_cast< std::size_t >( r ) ] == orig )
						{
							this->m_pick = r;
							break;
						}
					}
				}
			}

			void apply_filter( )
			{
				std::string query;
				query.reserve( this->m_search_buf.size( ) );
				for ( const char ch : this->m_search_buf )
				{
					query.push_back( static_cast< char >( std::tolower( static_cast< unsigned char >( ch ) ) ) );
				}

				if ( !this->m_pick_dirty && query == this->m_filter_cache && this->m_filter_built_for == this->m_cache_key )
				{
					return;
				}

				this->m_filter_cache = query;
				this->m_filter_built_for = this->m_cache_key;

				this->m_filtered_names.clear( );
				this->m_filtered_idx.clear( );
				this->m_filtered_names.emplace_back( "None" );
				this->m_filtered_idx.emplace_back( 0 );

				for ( auto i = 0; i < static_cast< int >( this->m_names.size( ) ); ++i )
				{
					const auto& n = this->m_names[ static_cast< std::size_t >( i ) ];

					if ( query.empty( ) )
					{
						this->m_filtered_names.emplace_back( n );
						this->m_filtered_idx.emplace_back( i + 1 );
						continue;
					}

					std::string low;
					low.reserve( n.size( ) );
					for ( const char ch : n )
					{
						low.push_back( static_cast< char >( std::tolower( static_cast< unsigned char >( ch ) ) ) );
					}

					if ( low.find( query ) != std::string::npos )
					{
						this->m_filtered_names.emplace_back( n );
						this->m_filtered_idx.emplace_back( i + 1 );
					}
				}

				this->m_filtered_ptrs.clear( );
				this->m_filtered_ptrs.reserve( this->m_filtered_names.size( ) );
				for ( const auto& n : this->m_filtered_names )
				{
					this->m_filtered_ptrs.push_back( n.c_str( ) );
				}

				this->m_pick_dirty = true;
			}

			[[nodiscard]] xui::rect get_popup( ) const
			{
				return { this->m_anchor.x, this->m_anchor.y, 280.0f, 376.0f };
			}

			std::int16_t m_def{};
			bool m_keychain_mode{};
			int m_slot{};
			int m_synced_slot{ -1 };
			int m_pick{};
			int m_cache_key{ -1 };
			int m_filter_built_for{ -1 };
			bool m_pick_dirty{};
			std::int16_t m_last_id{};
			std::vector<std::string> m_names{};
			std::vector<const char*> m_ptrs{};
			std::vector<std::string> m_filtered_names{};
			std::vector<int> m_filtered_idx{};
			std::vector<const char*> m_filtered_ptrs{};
			std::string m_search_buf{};
			std::string m_filter_cache{};
			std::string m_seed_buf{};
			float m_open_anim{};
		};

		class skin_context_overlay : public xui::overlay
		{
		public:
			skin_context_overlay( std::uintptr_t id, const xui::rect& anchor, std::int16_t def, std::string weapon_name, std::string skin_name ) : overlay{ id, anchor }, m_def{ def }, m_weapon_name{ std::move( weapon_name ) }, m_skin_name{ std::move( skin_name ) }
			{
				this->m_hover_anims.fill( 0.0f );
				this->m_item_anims.fill( 0.0f );

				const auto* item_def = features::changer::g_econ_item_system.find_def( def );
				this->m_is_glove = item_def && item_def->category == features::changer::econ_item_system::item_category::glove;
			}

			[[nodiscard]] bool hit_test( float x, float y ) const override
			{
				return this->get_popup( ).contains( x, y );
			}

			bool process_input( const xui::input_state& input ) override
			{
				if ( this->m_closing )
				{
					return false;
				}

				if ( this->m_sub_id != xui::null_id )
				{
					if ( const auto sub = xui::overlays::find( this->m_sub_id ) )
					{
						if ( sub->hit_test( input.mouse_x, input.mouse_y ) )
						{
							return false;
						}
					}
					else
					{
						this->m_sub_id = xui::null_id;
					}
				}

				const auto popup = this->get_popup( );

				if ( ( input.mouse_clicked || input.rmb_clicked ) && !popup.contains( input.mouse_x, input.mouse_y ) && !this->m_anchor.contains( input.mouse_x, input.mouse_y ) )
				{
					this->m_closing = true;

					if ( this->m_sub_id != xui::null_id )
					{
						xui::overlays::close( this->m_sub_id );
					}

					return true;
				}

				if ( !input.mouse_clicked || !popup.contains( input.mouse_x, input.mouse_y ) )
				{
					return popup.contains( input.mouse_x, input.mouse_y );
				}

				const auto item_count = this->item_count( );

				for ( auto slot = 0; slot < item_count; ++slot )
				{
					const auto ir = this->get_item_rect( popup, slot );
					if ( !ir.contains( input.mouse_x, input.mouse_y ) )
					{
						continue;
					}

					const auto id = this->item_id( slot );

					auto _smg9 = skin_map_guard{}; const auto it = _smg9.find( this->m_def );
					if ( it == _smg9.end( ) )
					{
						return true;
					}

					if ( id == 1 )
					{
						it->second.stattrak = !it->second.stattrak;
					}
					else if ( id >= 2 && id <= 8 )
					{
						if ( this->m_sub_id != xui::null_id )
						{
							xui::overlays::close( this->m_sub_id );
						}

						const auto sub_anchor = xui::rect{ popup.right( ) + 4.0f, ir.y, 0.0f, 0.0f };
						this->m_sub_id = ( id == 2 ) ? ( this->m_id ^ 0x77700ull )
							: ( id == 3 ) ? ( this->m_id ^ 0x77711ull )
							: ( id == 4 ) ? ( this->m_id ^ 0x77722ull )
							: ( id == 5 ) ? ( this->m_id ^ 0x77733ull )
							: ( id == 6 ) ? ( this->m_id ^ 0x77744ull )
							: ( id == 7 ) ? ( this->m_id ^ 0x77755ull )
							: ( this->m_id ^ 0x77766ull );

						if ( id == 2 )
						{
							xui::overlays::add( std::make_unique<stattrak_value_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
						else if ( id == 3 )
						{
							xui::overlays::add( std::make_unique<wear_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
						else if ( id == 4 )
						{
							xui::overlays::add( std::make_unique<seed_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
						else if ( id == 5 )
						{
							xui::overlays::add( std::make_unique<nametag_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
						else if ( id == 6 )
						{
							xui::overlays::add( std::make_unique<paint_color_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def ) );
						}
						else
						{
							xui::overlays::add( std::make_unique<sticker_keychain_sub_overlay>( this->m_sub_id, sub_anchor, this->m_def, id == 8 ) );
						}
					}
					else if ( id == 9 )
					{
						erase_applied_skin( this->m_def );

						this->m_closing = true;

						if ( this->m_sub_id != xui::null_id )
						{
							xui::overlays::close( this->m_sub_id );
						}
					}

					return true;
				}

				return true;
			}

			void render( const xui::style& style, const xui::input_state& input ) override
			{
				if ( this->m_sub_id != xui::null_id )
				{
					xui::overlays::touch( this->m_sub_id );
				}

				const auto dt = xdraw::delta_time( );
				const auto popup = this->get_popup( );
				auto& dl = xdraw::get( xdraw::layer::top );

				const auto speed = this->m_closing ? 18.0f : 16.0f;
				const auto target = this->m_closing ? 0.0f : 1.0f;
				this->m_open_anim += ( target - this->m_open_anim ) * std::min( speed * dt, 1.0f );

				if ( this->m_open_anim < 0.01f && this->m_closing )
				{
					this->m_closed = true;
					return;
				}

				const auto ease_t = xui::ease::out_cubic( this->m_open_anim );
				const auto alpha_mult = ease_t;
				const auto animated_h = popup.h * ease_t;
				const auto pr = style.popup_rounding;

				auto bg = style.popup_bg;
				bg.a = static_cast< std::uint8_t >( bg.a * alpha_mult );
				auto border = xui::lighten( style.popup_border, 1.1f );
				border.a = static_cast< std::uint8_t >( border.a * alpha_mult );

				
				dl.rect_filled( popup.x, popup.y, popup.w, animated_h, bg, xdraw::corner_radius{ pr } );
				dl.rect( popup.x, popup.y, popup.w, animated_h, border, xdraw::corner_radius{ pr } );

				dl.push_clip( popup.x, popup.y, popup.w, animated_h );

				// Snapshot under the map mutex so this render never walks a live
				// node whose strings could be mid-write (or replaced by a rehash)
				// on the game thread. Reading a copy also keeps the value strings
				// owned by this overlay for the whole frame.
				settings::changer::applied_skin entry{};
				bool entry_found = false;
				{
					std::lock_guard<std::recursive_mutex> lock{ settings::g_changer.skins.mx };
					const auto it = settings::g_changer.skins.data.find( this->m_def );
					if ( it != settings::g_changer.skins.data.end( ) )
					{
						entry = settings::changer::snapshot_skin( it->second );
						entry_found = true;
					}
				}

				const auto item_count = this->item_count( );

				for ( auto slot = 0; slot < item_count; ++slot )
				{
					const auto id = this->item_id( slot );
					const auto ir = this->get_item_rect( popup, slot );
					const auto item_delay = slot * 0.04f;
					const auto item_progress = std::clamp( ( this->m_open_anim - item_delay ) / ( 1.0f - std::min( item_delay, 0.3f ) ), 0.0f, 1.0f );
					auto& ia = this->m_item_anims[ slot ];
					ia = std::min( ia + 20.0f * dt, item_progress );

					const auto item_ease = xui::ease::out_cubic( ia );
					const auto item_alpha = item_ease * alpha_mult;
					const auto slide = ( 1.0f - item_ease ) * 6.0f;

					if ( slot == 0 )
					{
						char header[ 128 ]{};
						if ( !this->m_skin_name.empty( ) )
						{
							std::snprintf( header, sizeof( header ), "%s | %s", this->m_weapon_name.c_str( ), this->m_skin_name.c_str( ) );
						}
						else
						{
							std::snprintf( header, sizeof( header ), "%s", this->m_weapon_name.c_str( ) );
						}

						auto col = style.text;
						col.a = static_cast< std::uint8_t >( col.a * item_alpha );

						const auto trunc = xui::truncate( header, ir.w - 16.0f );
						const auto [tw, th] = xdraw::measure_text( trunc );
						dl.text( ir.x + 8.0f, ir.y + ( k_item_h - th ) * 0.5f + slide, trunc, col );
						continue;
					}

					if ( id == 9 )
					{
						auto sep = style.separator;
						sep.a = static_cast< std::uint8_t >( sep.a * item_alpha );
						dl.line( ir.x + 6.0f, ir.y + slide, ir.right( ) - 6.0f, ir.y + slide, sep, 1.0f );
					}

					const auto is_hovered = !this->m_closing && ir.contains( input.mouse_x, input.mouse_y );
					auto& ha = this->m_hover_anims[ slot ];
					ha += ( ( is_hovered ? 1.0f : 0.0f ) - ha ) * std::min( 18.0f * dt, 1.0f );

					if ( ha > 0.01f )
					{
						auto hov = style.combo_popup_item_hovered;
						hov.a = static_cast< std::uint8_t >( hov.a * item_alpha * ha );
						const auto first = ( slot == 0 );
						const auto last = ( slot == item_count - 1 );
						dl.rect_filled( ir.x, ir.y + slide, ir.w, ir.h, hov, xdraw::corner_radius{ first ? pr : 0.0f, first ? pr : 0.0f, last ? pr : 0.0f, last ? pr : 0.0f } );
					}

					if ( id == 9 )
					{
						auto col = xdraw::color{ 235, 75, 75, 230 };
						col = xui::lerp( col, xdraw::color{ 255, 100, 100, 255 }, ha );
						col.a = static_cast< std::uint8_t >( col.a * item_alpha );

						const auto [tw, th] = xdraw::measure_text( "Remove skin" );
						dl.text( ir.x + 8.0f, ir.y + ( k_item_h - th ) * 0.5f + slide, "Remove skin", col );
						continue;
					}

					static constexpr const char* labels[ ]{ "", "Stattrak", "StatTrak value", "Wear", "Seed", "Nametag", "Paint color", "Stickers", "Keychain", "Remove" };

					std::string value;
					if ( entry_found )
					{
						if ( id == 1 )
						{
							value = entry.stattrak ? "on" : "off";
						}
						else if ( id == 2 )
						{
							value = std::to_string( entry.stattrak_value );
						}
						else if ( id == 3 )
						{
							value = format_wear( entry.wear );
						}
						else if ( id == 4 )
						{
							value = std::to_string( entry.seed );
						}
						else if ( id == 5 )
						{
							value = ( !settings::changer::valid_nametag( entry.nametag ) || entry.nametag.empty( ) ) ? "none" : entry.nametag;
						}
						else if ( id == 6 )
						{
							value = entry.paint_color ? "on" : "off";
						}
						else if ( id == 7 )
						{
							auto filled = 0;
							for ( const auto& st : entry.stickers )
							{
								filled += ( st.id != 0 );
							}

							value = std::to_string( filled );
						}
						else if ( id == 8 )
						{
							value = entry.keychain.id != 0 ? "on" : "off";
						}
						else
						{
							value = "-";
						}
					}
					else
					{
						value = "-";
					}

					auto label_col = style.text_dim;
					label_col.a = static_cast< std::uint8_t >( label_col.a * item_alpha );

					auto val_col = style.text;
					val_col = xui::lerp( val_col, xui::lighten( val_col, 1.3f ), ha );
					val_col.a = static_cast< std::uint8_t >( val_col.a * item_alpha );

					const auto [lw, lh] = xdraw::measure_text( labels[ id ] );
					dl.text( ir.x + 8.0f, ir.y + ( k_item_h - lh ) * 0.5f + slide, labels[ id ], label_col );

					const auto [vw, vh] = xdraw::measure_text( value );
					dl.text( ir.right( ) - vw - 8.0f, ir.y + ( k_item_h - vh ) * 0.5f + slide, value, val_col );
				}

				dl.pop_clip( );
			}

		private:
			static constexpr auto k_item_count{ 10 };
			static constexpr auto k_item_h{ 24.0f };
			static constexpr auto k_pad{ 4.0f };
			static constexpr auto k_popup_w{ 200.0f };

			// Popup rows by item id (slot 0 is the header row). Gloves only
			// support the paint colour override - everything else is
			// weapon-specific.
			static constexpr int k_normal_items[]{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
			static constexpr int k_glove_items[]{ 0, 6 };

			[[nodiscard]] int item_count( ) const
			{
				return this->m_is_glove ? 2 : static_cast< int >( std::size( k_normal_items ) );
			}

			[[nodiscard]] int item_id( int slot ) const
			{
				return this->m_is_glove ? k_glove_items[ slot ] : k_normal_items[ slot ];
			}

			[[nodiscard]] xui::rect get_popup( ) const
			{
				const auto h = k_pad * 2.0f + k_item_h * static_cast< float >( this->item_count( ) );
				return { this->m_anchor.x, this->m_anchor.bottom( ) + 4.0f, k_popup_w, h };
			}

			[[nodiscard]] xui::rect get_item_rect( const xui::rect& popup, int index ) const
			{
				return { popup.x + k_pad, popup.y + k_pad + static_cast< float >( index ) * k_item_h, popup.w - k_pad * 2.0f, k_item_h };
			}

			std::int16_t m_def{};
			bool m_is_glove{};
			std::string m_weapon_name{};
			std::string m_skin_name{};
			std::uintptr_t m_sub_id{ xui::null_id };
			float m_open_anim{};
			std::array<float, k_item_count> m_hover_anims{};
			std::array<float, k_item_count> m_item_anims{};
		};

		static inline void draw_agent_team_card( const xui::rect& card, int team, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto& sel = settings::g_changer.agents;
			const auto selected_def = ( team == 3 ) ? sel.ct_def : sel.t_def;
			const auto def = ( selected_def != 0 ) ? econ.find_def( selected_def ) : nullptr;

			const features::changer::econ_item_system::item_def* default_agent{ nullptr };
			const auto default_name = ( team == 3 ) ? "Default CT Agent" : "Default T Agent";

			for ( const auto a : econ.agents( ) )
			{
				if ( a && a->localized_name == default_name )
				{
					default_agent = a;
					break;
				}
			}

			const auto image_h = std::floor( card.h * k_image_h_ratio );
			const auto hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( card );
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "ateam" ) + static_cast< std::uintptr_t >( team ), hovered ? 1.0f : 0.0f, 14.0f );

			auto card_bg = tokens::col_card;
			card_bg = xui::lerp( card_bg, xui::lighten( card_bg, 1.4f ), hover_anim * 0.5f );
			card_bg.a = static_cast< std::uint8_t >( card_bg.a * fade_alpha );
			dl.rect_filled( card.x, card.y, card.w, card.h, card_bg, xdraw::corner_radius{ tokens::btn_rounding } );

			if ( def )
			{
				auto bcol = tokens::col_accent;
				bcol.a = static_cast< std::uint8_t >( bcol.a * fade_alpha );
				dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.0f );
			}

			const auto img_source = def ? def : default_agent;
			if ( img_source )
			{
				const auto img = econ.get_skin_image( img_source->image_inventory );
				if ( img )
				{
					const auto target_h = image_h - 12.0f;
					const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
					auto iw = target_h * aspect;
					auto ih = target_h;

					if ( iw > card.w - 12.0f )
					{
						iw = card.w - 12.0f;
						ih = iw / aspect;
					}

					const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
					const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f );
					const auto alpha = def ? 255.0f : 100.0f;
					const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( alpha * fade_alpha ) };

					dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
				}
			}

			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;
			const auto label = def ? def->localized_name : ( team == 3 ? "CT" : "T" );

			auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

			const auto ntrunc = xui::truncate( label, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( ntrunc );
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );

			if ( def )
			{
				const auto team_label = ( team == 3 ) ? "CT" : "T";
				auto team_col = tokens::col_text_dim;
				team_col.a = static_cast< std::uint8_t >( 180.0f * fade_alpha );
				dl.text( card.x + 6.0f, card.y + 4.0f, team_label, team_col );
			}

			if ( hovered && input.mouse_clicked )
			{
				skins_ui.browsing_agent_team = team;
				request_page( skins_page::browser, 0 );
			}
		}

		static inline void draw_weapon_card( const xui::rect& card, const features::changer::econ_item_system::item_def* def, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			auto _smg_wcard = skin_map_guard{}; const auto applied_it = _smg_wcard.find( def->def_index );
			const auto is_skinned = ( applied_it != _smg_wcard.end( ) );

			const auto pk = ( is_skinned && applied_it->second.paint_kit_id != 0 ) ? econ.find_paint_kit( applied_it->second.paint_kit_id ) : nullptr;
			const auto rarity = pk ? econ.combined_rarity( def->def_index, applied_it->second.paint_kit_id ) : 0;
			const auto rarity_col = k_rarity_colors[ std::clamp( rarity, 0, 7 ) ];

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( card );
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "wcard" ) + static_cast< std::uintptr_t >( def->def_index ), hovered ? 1.0f : 0.0f, 14.0f );

			auto card_bg = tokens::col_card;
			card_bg = xui::lerp( card_bg, xui::lighten( card_bg, 1.4f ), hover_anim * 0.5f );
			card_bg.a = static_cast< std::uint8_t >( card_bg.a * fade_alpha );
			dl.rect_filled( card.x, card.y, card.w, card.h, card_bg, xdraw::corner_radius{ tokens::btn_rounding } );

			if ( is_skinned )
			{
				if ( pk )
				{
					auto bcol = rarity_col;
					bcol.a = static_cast< std::uint8_t >( ( 100.0f + 80.0f * hover_anim ) * fade_alpha );
					dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.0f );
				}
				else
				{
					auto bcol = tokens::col_accent;
					bcol.a = static_cast< std::uint8_t >( bcol.a * fade_alpha );
					dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.0f );
				}
			}

			const auto img = ( is_skinned && pk ) ? econ.get_skin_image( def->def_index, applied_it->second.paint_kit_id ) : econ.get_skin_image( def->image_inventory );
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 12.0f )
				{
					iw = card.w - 12.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}
			else if ( def->category == features::changer::econ_item_system::item_category::glove && !img )
			{
				for ( const auto& skin : econ.skins( ) )
				{
					if ( skin.def_index != def->def_index )
					{
						continue;
					}

					const auto fallback_img = econ.get_skin_image( def->def_index, skin.paint_kit_id );
					if ( fallback_img )
					{
						const auto target_h = image_h - 12.0f;
						const auto aspect = static_cast< float >( fallback_img->width ) / static_cast< float >( fallback_img->height );
						auto iw = target_h * aspect;
						auto ih = target_h;

						if ( iw > card.w - 12.0f )
						{
							iw = card.w - 12.0f;
							ih = iw / aspect;
						}

						const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
						const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f );
						const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 150.0f * fade_alpha ) };

						dl.image( ix, iy, iw, ih, fallback_img->srv.Get( ), tint );
						break;
					}
				}
			}

			if ( pk )
			{
				auto bar = rarity_col;
				bar.a = static_cast< std::uint8_t >( bar.a * fade_alpha );
				dl.rect_filled( card.x + 6.0f, card.y + image_h, card.w - 12.0f, k_rarity_bar_h, bar );
			}

			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;

			if ( pk )
			{
				auto skin_col = rarity_col;
				skin_col.a = static_cast< std::uint8_t >( 200.0f * fade_alpha );

				const auto skin_trunc = xui::truncate( pk->localized_name, card.w - 12.0f );
				const auto [sw, sh] = xdraw::measure_text( skin_trunc );
				dl.text( std::floor( card.x + ( card.w - sw ) * 0.5f ), std::floor( name_y ), skin_trunc, skin_col );
			}
			else
			{
				auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
				ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

				const auto ntrunc = xui::truncate( def->localized_name, card.w - 12.0f );
				const auto [nw2, nh2] = xdraw::measure_text( ntrunc );
				dl.text( std::floor( card.x + ( card.w - nw2 ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );
			}

			// "Modify" button under the text for skinned items (replaces right-click context menu).
			// Gloves get a reduced overlay (wear/seed/paint colour only).
			if ( is_skinned )
			{
				constexpr auto modify_h{ 16.0f };
				constexpr auto modify_w{ 64.0f };
				const auto modify_x = card.x + ( card.w - modify_w ) * 0.5f;
				const auto modify_y = card.bottom( ) - modify_h - 5.0f;
				const auto modify_rect = xui::rect{ modify_x, modify_y, modify_w, modify_h };

				const auto ctx_id = xui::fnv1a( "skin_ctx" ) ^ static_cast< std::uintptr_t >( def->def_index );
				const auto ctx_is_open = xui::overlays::is_open( ctx_id );

				const auto modify_hovered = ( !xui::ctx( ).overlay_blocking( ) || ctx_is_open ) && input.in_rect( modify_rect );
				const auto modify_anim = xui::anim::lerp( xui::fnv1a( "wmodify" ) + static_cast< std::uintptr_t >( def->def_index ), modify_hovered ? 1.0f : 0.0f, 14.0f );

				auto btn_bg = xui::lerp( tokens::col_card, xui::lighten( tokens::col_card, 1.5f ), modify_anim );
				btn_bg.a = static_cast< std::uint8_t >( btn_bg.a * fade_alpha );
				dl.rect_filled( modify_rect.x, modify_rect.y, modify_rect.w, modify_rect.h, btn_bg );

				auto btn_border = pk ? rarity_col : tokens::col_accent;
				btn_border.a = static_cast< std::uint8_t >( ( 60.0f + 120.0f * modify_anim ) * fade_alpha );
				dl.rect( modify_rect.x, modify_rect.y, modify_rect.w, modify_rect.h, btn_border, xdraw::corner_radius{ 0.0f }, 1.0f );

				const auto [mw, mh] = xdraw::measure_text( "Modify" );
				auto btn_text = xui::lerp( tokens::col_text_dim, tokens::col_text, modify_anim );
				btn_text.a = static_cast< std::uint8_t >( btn_text.a * fade_alpha );
				dl.text( modify_rect.x + ( modify_rect.w - mw ) * 0.5f, modify_rect.y + ( modify_rect.h - mh ) * 0.5f, "Modify", btn_text );

				if ( modify_hovered && input.mouse_clicked )
				{
					if ( ctx_is_open )
					{
						xui::overlays::close( ctx_id );
					}
					else
					{
						const auto skin_title = pk ? pk->localized_name : std::string{};
						xui::overlays::add( std::make_unique<skin_context_overlay>( ctx_id, modify_rect, def->def_index, def->localized_name, skin_title ) );
					}

					return;
				}
			}

			if ( is_skinned && !pk )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( !hovered )
			{
				return;
			}

			if ( input.mouse_clicked )
			{
				if ( def->category == features::changer::econ_item_system::item_category::agent )
				{
					if ( is_skinned )
					{
						erase_applied_skin( def->def_index );
					}
					else
					{
						for ( const auto* a : econ.agents( ) )
						{
							erase_applied_skin( a->def_index );
						}

						mutate_applied_skin( def->def_index, []( auto& a )
						{
							a.paint_kit_id = 0;
							a.wear = 0.0f;
							a.seed = 0;
							a.stattrak = false;
						} );
					}
				}
				else
				{
					request_page( skins_page::browser, def->def_index );
				}

				return;
			}
		}

		static inline void draw_agent_tile( const xui::rect& card, const features::changer::econ_item_system::item_def* def, bool is_equipped, float fade_alpha, bool is_default_placeholder = false )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( card );
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "atile" ) + static_cast< std::uintptr_t >( def->def_index ), hovered ? 1.0f : 0.0f, 14.0f );

			auto card_bg = tokens::col_card;
			card_bg = xui::lerp( card_bg, xui::lighten( card_bg, 1.4f ), hover_anim * 0.5f );
			card_bg.a = static_cast< std::uint8_t >( card_bg.a * fade_alpha );
			dl.rect_filled( card.x, card.y, card.w, card.h, card_bg, xdraw::corner_radius{ tokens::btn_rounding } );

			if ( is_equipped )
			{
				auto bcol = tokens::col_accent;
				bcol.a = static_cast< std::uint8_t >( bcol.a * fade_alpha );
				dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.5f );
			}

			const auto img = econ.get_skin_image( def->image_inventory );
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 12.0f )
				{
					iw = card.w - 12.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}

			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;

			auto ncol = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			ncol.a = static_cast< std::uint8_t >( ncol.a * fade_alpha );

			const auto ntrunc = xui::truncate( def->localized_name, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( ntrunc );
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), ntrunc, ncol );

			if ( is_equipped )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( hovered && input.mouse_clicked )
			{
				auto& target = ( skins_ui.browsing_agent_team == 3 ) ? settings::g_changer.agents.ct_def : settings::g_changer.agents.t_def;

				if ( is_default_placeholder || is_equipped )
				{
					target = 0;
				}
				else
				{
					target = def->def_index;
				}

				request_page( skins_page::grid );
			}
		}

		static inline void draw_skin_tile( const xui::rect& card, const features::changer::econ_item_system::paint_kit* pk, const features::changer::econ_item_system::item_def* weapon, int current_kit_id, float fade_alpha )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto rarity = weapon ? econ.combined_rarity( weapon->def_index, pk->id ) : pk->rarity;
			const auto rarity_col = k_rarity_colors[ std::clamp( rarity, 0, 7 ) ];

			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto is_equipped = ( pk->id == current_kit_id );
			const auto hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( card );
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( "scard" ) + static_cast< std::uintptr_t >( pk->id ), hovered ? 1.0f : 0.0f, 14.0f );

			auto card_bg = tokens::col_card;
			card_bg = xui::lerp( card_bg, xui::lighten( card_bg, 1.4f ), hover_anim * 0.5f );
			card_bg.a = static_cast< std::uint8_t >( card_bg.a * fade_alpha );
			dl.rect_filled( card.x, card.y, card.w, card.h, card_bg, xdraw::corner_radius{ tokens::btn_rounding } );

			if ( is_equipped )
			{
				auto bcol = tokens::col_accent;
				bcol.a = static_cast< std::uint8_t >( bcol.a * fade_alpha );
				dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.5f );
			}

			const auto img = weapon ? econ.get_skin_image( weapon->def_index, pk->id ) : nullptr;
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 12.0f )
				{
					iw = card.w - 12.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}
			else
			{
				auto ph_col = xui::darken( tokens::col_card, 0.7f );
				ph_col.a = static_cast< std::uint8_t >( 100.0f * fade_alpha );
				dl.rect_filled( card.x + 6.0f, card.y + 6.0f, card.w - 12.0f, image_h - 12.0f, ph_col );
			}

			auto bar = rarity_col;
			bar.a = static_cast< std::uint8_t >( bar.a * fade_alpha );
			dl.rect_filled( card.x + 6.0f, card.y + image_h, card.w - 12.0f, k_rarity_bar_h, bar );

			auto name_col = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			name_col.a = static_cast< std::uint8_t >( name_col.a * fade_alpha );

			const auto name_trunc = xui::truncate( pk->localized_name, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( name_trunc );
			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), name_trunc, name_col );

			if ( is_equipped )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			if ( hovered && input.mouse_clicked )
			{
				if ( is_equipped )
				{
					const auto browsing_def = econ.find_def( skins_ui.browsing_def );
					if ( !browsing_def || browsing_def->category != features::changer::econ_item_system::item_category::agent )
					{
						return;
					}

					erase_applied_skin( skins_ui.browsing_def );
					request_page( skins_page::grid );
					return;
				}
				else
				{
					const auto browsing_def = econ.find_def( skins_ui.browsing_def );
					if ( browsing_def )
					{
						if ( browsing_def->category == features::changer::econ_item_system::item_category::knife )
						{
							for ( const auto* k : econ.knives( ) )
							{
								erase_applied_skin( k->def_index );
							}
						}
						else if ( browsing_def->category == features::changer::econ_item_system::item_category::glove )
						{
							for ( const auto* g : econ.gloves( ) )
							{
								erase_applied_skin( g->def_index );
							}
						}
					}

					mutate_applied_skin( skins_ui.browsing_def, [&]( auto& a )
					{
						a.paint_kit_id = pk->id;
						a.wear = 0.01f;
						a.seed = 0;
						a.stattrak = false;
					} );
				}

				request_page( skins_page::grid );
			}
		}

		// Kit tile for music kits: icon, optional rarity bar, selection
		// border + check badge. Returns true when clicked.
		static inline bool draw_kit_tile( const xui::rect& card, const features::changer::econ_item_system::kit_entry& kit, bool selected, float fade_alpha, std::string_view anim_seed )
		{
			auto& econ = features::changer::g_econ_item_system;
			auto& dl = xui::draw::current( );
			const auto& input = xui::ctx( ).input;

			const auto rarity_col = k_rarity_colors[ std::clamp( static_cast< int >( kit.rarity ), 0, 7 ) ];
			const auto image_h = std::floor( card.h * k_image_h_ratio );

			const auto hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( card );
			const auto hover_anim = xui::anim::lerp( xui::fnv1a( anim_seed ) + static_cast< std::uintptr_t >( kit.id ), hovered ? 1.0f : 0.0f, 14.0f );

			auto card_bg = tokens::col_card;
			card_bg = xui::lerp( card_bg, xui::lighten( card_bg, 1.4f ), hover_anim * 0.5f );
			card_bg.a = static_cast< std::uint8_t >( card_bg.a * fade_alpha );
			dl.rect_filled( card.x, card.y, card.w, card.h, card_bg, xdraw::corner_radius{ tokens::btn_rounding } );

			if ( selected )
			{
				auto bcol = tokens::col_accent;
				bcol.a = static_cast< std::uint8_t >( bcol.a * fade_alpha );
				dl.rect( card.x, card.y, card.w, card.h, bcol, xdraw::corner_radius{ tokens::btn_rounding }, 1.5f );
			}

			const auto img = kit.image_inventory.empty( ) ? nullptr : econ.get_skin_image( kit.image_inventory );
			if ( img )
			{
				const auto target_h = image_h - 12.0f;
				const auto aspect = static_cast< float >( img->width ) / static_cast< float >( img->height );
				auto iw = target_h * aspect;
				auto ih = target_h;

				if ( iw > card.w - 12.0f )
				{
					iw = card.w - 12.0f;
					ih = iw / aspect;
				}

				const auto ix = std::floor( card.x + ( card.w - iw ) * 0.5f );
				const auto iy = std::floor( card.y + ( image_h - ih ) * 0.5f );
				const auto tint = xdraw::color{ 255, 255, 255, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				dl.image( ix, iy, iw, ih, img->srv.Get( ), tint );
			}
			else
			{
				auto ph_col = xui::darken( tokens::col_card, 0.7f );
				ph_col.a = static_cast< std::uint8_t >( 100.0f * fade_alpha );
				dl.rect_filled( card.x + 6.0f, card.y + 6.0f, card.w - 12.0f, image_h - 12.0f, ph_col );
			}

			if ( kit.rarity > 0 )
			{
				auto bar = rarity_col;
				bar.a = static_cast< std::uint8_t >( bar.a * fade_alpha );
				dl.rect_filled( card.x + 6.0f, card.y + image_h, card.w - 12.0f, k_rarity_bar_h, bar );
			}

			auto name_col = xui::lerp( tokens::col_text_dim, tokens::col_text, hover_anim );
			name_col.a = static_cast< std::uint8_t >( name_col.a * fade_alpha );

			const auto name_trunc = xui::truncate( kit.localized_name.empty( ) ? kit.name : kit.localized_name, card.w - 12.0f );
			const auto [nw, nh] = xdraw::measure_text( name_trunc );
			const auto name_y = card.y + image_h + k_rarity_bar_h + 4.0f;
			dl.text( std::floor( card.x + ( card.w - nw ) * 0.5f ), std::floor( name_y ), name_trunc, name_col );

			if ( selected )
			{
				constexpr auto badge{ 14.0f };
				const auto bx = card.right( ) - badge - 4.0f;
				const auto by = card.y + 4.0f;

				auto badge_bg = tokens::col_accent;
				badge_bg.a = static_cast< std::uint8_t >( badge_bg.a * fade_alpha );
				dl.rect_filled( bx, by, badge, badge, badge_bg, xdraw::corner_radius{ badge * 0.5f } );

				const auto cx = bx + badge * 0.5f;
				const auto cy = by + badge * 0.5f;
				const auto check = xdraw::color{ tokens::col_dark.r, tokens::col_dark.g, tokens::col_dark.b, static_cast< std::uint8_t >( 255.0f * fade_alpha ) };

				const std::array<float, 6> pts
				{
					cx - badge * 0.20f, cy,
					cx - badge * 0.05f, cy + badge * 0.18f,
					cx + badge * 0.25f, cy - badge * 0.18f
				};

				dl.polyline( pts, check, false, 1.5f );
			}

			return hovered && input.mouse_clicked;
		}

	} // namespace detail

	void menu::draw_skins( float group_w ) const
	{
		static auto last_subtab{ -1 };
		if ( this->m_subtab != last_subtab )
		{
			last_subtab = this->m_subtab;
			detail::skins_ui.current = detail::skins_page::grid;
			detail::skins_ui.target = detail::skins_page::grid;
			detail::skins_ui.fade = 1.0f;
			detail::skins_ui.browsing_agent_team = 0;
			detail::skins_ui.search_buf.clear( );
		}

		// Custom model selector
		static int custom_model_idx{ 0 };
		static bool custom_model_init{ false };

		auto& changer = *GetModelChanger( );
		if ( !custom_model_init )
		{
			changer.OnInit( );
			custom_model_init = true;
		}

		const auto& models = changer.GetModels( );
		if ( models.size( ) > 1 )
		{
			std::vector<const char*> model_ptrs;
			model_ptrs.reserve( models.size( ) );

			for ( const auto& entry : models )
			{
				model_ptrs.push_back( entry.m_Name.c_str( ) );
			}

			if ( xui::combo( "Custom Model", custom_model_idx, model_ptrs.data( ), static_cast< int >( model_ptrs.size( ) ) ) )
			{
				changer.GetSelectedIdx( ) = custom_model_idx;

				if ( custom_model_idx > 0 )
				{
					changer.NeedSetModel( ) = true;
				}
			}
		}

		auto& econ = features::changer::g_econ_item_system;
		auto& dl = xui::draw::current( );
		const auto& s = xui::ctx( ).style;
		const auto& input = xui::ctx( ).input;

		const auto wx = this->m_x;
		const auto wy = this->m_y;
		const auto content_x = this->m_content_x;
		const auto body_y = this->m_body_y;
		const auto content_w = this->m_content_w;
		const auto body_h = this->m_body_h;

		// Page transitions are instant to avoid visual jitter while scrolling.
		// Both pages are independent children, so cross-fading is unnecessary.
		detail::skins_ui.current = detail::skins_ui.target;
		detail::skins_ui.fade = 1.0f;
		const auto fade_alpha = 1.0f;

		xui::layout::set_cursor( content_x - wx, body_y - wy );

		if ( detail::skins_ui.current == detail::skins_page::grid )
		{
			if ( !xui::begin_child( "##skins_grid", content_w, body_h, true ) )
			{
				return;
			}

			const auto win = xui::layout::current_window( );
			// Always reserve scrollbar space so the grid width is stable while scrolling
			// (prevents card reflow / horizontal jitter when the scrollbar appears or disappears).
			constexpr auto k_sb_w{ 6.0f };
			constexpr auto k_sb_pad{ 4.0f };
			const auto scrollbar_w = ( k_sb_w + k_sb_pad );
			const auto inner_w = win->bounds.w - s.window_pad_x * 2.0f - scrollbar_w;

			const auto total_gap = ( detail::k_columns - 1 ) * detail::k_card_gap;
			const auto card_w = std::floor( ( inner_w - total_gap ) / static_cast< float >( detail::k_columns ) );
			const auto card_h = std::floor( card_w * ( detail::k_card_h_ref / detail::k_card_w_ref ) );

			const auto grid_w = card_w * detail::k_columns + total_gap;
			const auto offset_x = std::max( 0.0f, ( inner_w - grid_w ) * 0.5f );

			if ( this->m_subtab == 3 )
			{
				const auto base_x = win->bounds.x + s.window_pad_x + offset_x;
				const auto base_y = win->bounds.y + s.window_pad_y - win->scroll_y;

				const auto ct_card = xui::rect{ base_x, base_y, card_w, card_h };
				const auto t_card = xui::rect{ base_x + card_w + detail::k_card_gap, base_y, card_w, card_h };

				detail::draw_agent_team_card( ct_card, 3, fade_alpha );
				detail::draw_agent_team_card( t_card, 2, fade_alpha );

				xui::layout::item( inner_w, card_h );
				xui::end_child( );
				return;
			}

			const auto& weapons = detail::select_weapons( this->m_subtab );

			std::unordered_set<std::int16_t> defs_with_skins;
			for ( const auto& skin : econ.skins( ) )
			{
				defs_with_skins.insert( skin.def_index );
			}

			std::vector<const features::changer::econ_item_system::item_def*> items;
			items.reserve( weapons.size( ) );

			for ( const auto* w : weapons )
			{
				if ( detail::is_hidden_def( w ) )
				{
					continue;
				}

				if ( ( w->category == features::changer::econ_item_system::item_category::knife || w->category == features::changer::econ_item_system::item_category::glove ) && !defs_with_skins.contains( w->def_index ) )
				{
					continue;
				}

				if ( !w->image_inventory.empty( ) || defs_with_skins.contains( w->def_index ) )
				{
					items.push_back( w );
				}
			}

			const auto rows = ( static_cast< int >( items.size( ) ) + detail::k_columns - 1 ) / detail::k_columns;
			const auto total_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );

			const auto base_x = win->bounds.x + s.window_pad_x + offset_x;
			const auto base_y = win->bounds.y + s.window_pad_y - win->scroll_y;

			xui::layout::item( inner_w, total_h );

			for ( auto i = 0; i < static_cast< int >( items.size( ) ); ++i )
			{
				const auto col = i % detail::k_columns;
				const auto row = i / detail::k_columns;

				const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
				const auto cy = std::floor( base_y + row * ( card_h + detail::k_card_gap ) );

				if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) )
				{
					continue;
				}

				const auto card = xui::rect{ cx, cy, card_w, card_h };
				detail::draw_weapon_card( card, items[ i ], fade_alpha );
			}

			xui::end_child( );
		}
		else
		{
			if ( !xui::begin_child( "##skins_browser", content_w, body_h, true ) )
			{
				return;
			}

			const auto win = xui::layout::current_window( );
			// Always reserve scrollbar space so the grid width is stable while scrolling
			// (prevents card reflow / horizontal jitter when the scrollbar appears or disappears).
			constexpr auto k_sb_w{ 6.0f };
			constexpr auto k_sb_pad{ 4.0f };
			const auto scrollbar_w = ( k_sb_w + k_sb_pad );
			const auto inner_w = win->bounds.w - s.window_pad_x * 2.0f - scrollbar_w;

			const auto total_gap = ( detail::k_columns - 1 ) * detail::k_card_gap;
			const auto card_w = std::floor( ( inner_w - total_gap ) / static_cast< float >( detail::k_columns ) );
			const auto card_h = std::floor( card_w * ( detail::k_card_h_ref / detail::k_card_w_ref ) );

			const auto grid_w = card_w * detail::k_columns + total_gap;
			const auto offset_x = std::max( 0.0f, ( inner_w - grid_w ) * 0.5f );

			constexpr auto bar_h{ 26.0f };
			const auto bar_x = win->bounds.x + s.window_pad_x;
			const auto bar_y = win->bounds.y + s.window_pad_y - win->scroll_y;

			const auto back_w{ 60.0f };
			const auto back_rect = xui::rect{ bar_x, bar_y, back_w, bar_h };
			const auto back_hovered = !xui::ctx( ).overlay_blocking( ) && input.in_rect( back_rect );
			const auto back_hover = xui::anim::lerp( xui::fnv1a( "skin_back" ), back_hovered ? 1.0f : 0.0f, 14.0f );

			if ( back_hovered && input.mouse_clicked )
			{
				detail::request_page( detail::skins_page::grid );
				detail::skins_ui.search_buf.clear( );
			}

			auto back_bg = xui::lerp( s.button_bg, s.button_hovered, back_hover );
			back_bg.a = static_cast< std::uint8_t >( back_bg.a * fade_alpha );
			dl.rect_filled( back_rect.x, back_rect.y, back_rect.w, back_rect.h, back_bg, xdraw::corner_radius{ s.button_rounding } );

			const auto [bw, bh] = xdraw::measure_text( "Back" );
			auto back_text = xui::lerp( s.text_dim, s.text, back_hover );
			back_text.a = static_cast< std::uint8_t >( back_text.a * fade_alpha );
			dl.text( back_rect.x + ( back_rect.w - bw ) * 0.5f, back_rect.y + ( back_rect.h - bh ) * 0.5f, "Back", back_text );

			xui::layout::set_cursor( back_rect.right( ) + 6.0f - win->bounds.x, bar_y - win->bounds.y );
			xui::text_input( "##skin_search", detail::skins_ui.search_buf, 64, "Search..." );

			const auto grid_top_y = bar_y + bar_h + 12.0f;
			const auto base_x = win->bounds.x + s.window_pad_x + offset_x;

			if ( detail::skins_ui.browsing_agent_team != 0 )
			{
				std::string search_lower = detail::skins_ui.search_buf;
				for ( auto& c : search_lower )
				{
					c = static_cast< char >( std::tolower( c ) );
				}

				std::vector<const features::changer::econ_item_system::item_def*> agent_items;

				const auto default_agent_name = ( detail::skins_ui.browsing_agent_team == 3 ) ? "Default CT Agent" : "Default T Agent";
				const features::changer::econ_item_system::item_def* default_placeholder{ nullptr };

				for ( const auto* a : econ.agents( ) )
				{
					if ( a && a->localized_name == default_agent_name )
					{
						default_placeholder = a;
						break;
					}
				}

				if ( default_placeholder && search_lower.empty( ) )
				{
					agent_items.push_back( default_placeholder );
				}

				for ( const auto* a : econ.agents( ) )
				{
					if ( detail::is_hidden_def( a ) )
					{
						continue;
					}

					const auto agent_team = a->team( );
					if ( agent_team != 0 && agent_team != detail::skins_ui.browsing_agent_team )
					{
						continue;
					}

					if ( !search_lower.empty( ) )
					{
						std::string n = a->localized_name;
						for ( auto& c : n )
						{
							c = static_cast< char >( std::tolower( c ) );
						}

						if ( n.find( search_lower ) == std::string::npos )
						{
							continue;
						}
					}

					agent_items.push_back( a );
				}

				const auto rows = ( static_cast< int >( agent_items.size( ) ) + detail::k_columns - 1 ) / detail::k_columns;
				const auto grid_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );

				xui::layout::set_cursor( s.window_pad_x, s.window_pad_y );
				xui::layout::item( inner_w, ( bar_h + 12.0f ) + grid_h );

				const auto& sel = settings::g_changer.agents;
				const auto current_agent = ( detail::skins_ui.browsing_agent_team == 3 ) ? sel.ct_def : sel.t_def;

				for ( auto i = 0; i < static_cast< int >( agent_items.size( ) ); ++i )
				{
					const auto col = i % detail::k_columns;
					const auto row = i / detail::k_columns;

					const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
					const auto cy = std::floor( grid_top_y + row * ( card_h + detail::k_card_gap ) );

					if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) )
					{
						continue;
					}

					const auto card = xui::rect{ cx, cy, card_w, card_h };
					const auto is_default_placeholder = ( agent_items[ i ] == default_placeholder );
					const auto is_equipped = is_default_placeholder ? ( current_agent == 0 ) : ( agent_items[ i ]->def_index == current_agent );
					detail::draw_agent_tile( card, agent_items[ i ], is_equipped, fade_alpha, is_default_placeholder );
				}

				win->content_h = ( s.window_pad_y + bar_h + 12.0f + grid_h ) - win->scroll_y;
				xui::end_child( );
				return;
			}

			const auto weapon = econ.find_def( detail::skins_ui.browsing_def );

			std::unordered_set<int> valid_kits;
			for ( const auto& skin : econ.skins( ) )
			{
				if ( skin.def_index == detail::skins_ui.browsing_def )
				{
					valid_kits.insert( skin.paint_kit_id );
				}
			}

			std::string search_lower = detail::skins_ui.search_buf;
			for ( auto& c : search_lower )
			{
				c = static_cast< char >( std::tolower( c ) );
			}

			std::vector<const features::changer::econ_item_system::paint_kit*> kits;
			kits.reserve( valid_kits.size( ) );

			for ( const auto& pk : econ.paint_kits( ) )
			{
				if ( valid_kits.find( pk.id ) == valid_kits.end( ) )
				{
					continue;
				}

				if ( !search_lower.empty( ) )
				{
					std::string n = pk.localized_name;
					for ( auto& c : n )
					{
						c = static_cast< char >( std::tolower( c ) );
					}

					if ( n.find( search_lower ) == std::string::npos )
					{
						continue;
					}
				}

				kits.push_back( &pk );
			}

			const auto rows = ( static_cast< int >( kits.size( ) ) + detail::k_columns - 1 ) / detail::k_columns;
			const auto grid_h = rows * card_h + ( rows > 0 ? ( rows - 1 ) * detail::k_card_gap : 0.0f );

			const auto reserved_h = ( bar_h + 12.0f ) + grid_h;
			xui::layout::set_cursor( s.window_pad_x, s.window_pad_y );
			xui::layout::item( inner_w, reserved_h );

			auto _smg_browser = detail::skin_map_guard{}; const auto applied_it = _smg_browser.find( detail::skins_ui.browsing_def );
			const auto current_kit = ( applied_it != _smg_browser.end( ) ) ? applied_it->second.paint_kit_id : -1;

			for ( auto i = 0; i < static_cast< int >( kits.size( ) ); ++i )
			{
				const auto col = i % detail::k_columns;
				const auto row = i / detail::k_columns;

				const auto cx = std::floor( base_x + col * ( card_w + detail::k_card_gap ) );
				const auto cy = std::floor( grid_top_y + row * ( card_h + detail::k_card_gap ) );

				if ( cy + card_h < win->bounds.y || cy > win->bounds.bottom( ) )
				{
					continue;
				}

				const auto card = xui::rect{ cx, cy, card_w, card_h };
				detail::draw_skin_tile( card, kits[ i ], weapon, current_kit, fade_alpha );
			}

			const auto total_content_h = s.window_pad_y + bar_h + 12.0f + grid_h;
			win->content_h = total_content_h - win->scroll_y;

			xui::end_child( );
		}
	}

} // namespace rendering
