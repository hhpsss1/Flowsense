#pragma once

#include <windows.h>
#include <wrl/client.h>
#include <d3d11.h>

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <span>
#include <utility>
#include <string_view>

namespace xdraw {

	enum class layer
	{
		bottom,
		middle,
		top,
		count
	};

	enum class text_style
	{
		normal,
		outlined,
		shadowed
	};

	struct color
	{
		union
		{
			std::uint32_t val;
			struct { std::uint8_t r, g, b, a; };
		};

		constexpr color( ) : val{ 0 } {}
		constexpr color( std::uint32_t v ) : val{ v } {}
		constexpr color( std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255 ) : r{ r }, g{ g }, b{ b }, a{ a } {}

		constexpr color alpha( std::uint8_t a_ ) const { return color{ r, g, b, a_ }; }
		constexpr operator std::uint32_t( ) const { return val; }
		constexpr std::array<float, 4> to_float( ) const { return { r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f }; }
	};

	struct corner_radius
	{
		float tl{}, tr{}, br{}, bl{};

		constexpr corner_radius( ) = default;
		constexpr corner_radius( float all ) : tl{ all }, tr{ all }, br{ all }, bl{ all } {}
		constexpr corner_radius( float tl, float tr, float br, float bl ) : tl{ tl }, tr{ tr }, br{ br }, bl{ bl } {}

		static constexpr corner_radius top( float r ) { return { r, r, 0.0f, 0.0f }; }
		static constexpr corner_radius bottom( float r ) { return { 0.0f, 0.0f, r, r }; }
		static constexpr corner_radius left( float r ) { return { r, 0.0f, 0.0f, r }; }
		static constexpr corner_radius right( float r ) { return { 0.0f, r, r, 0.0f }; }
	};

	struct vertex
	{
		float pos[ 2 ];
		float uv[ 2 ];
		color col;
	};

	struct draw_cmd
	{
		std::uint32_t idx_offset{};
		std::uint32_t idx_count{};
		ID3D11ShaderResourceView* texture{};
		D3D11_RECT scissor{};
		bool has_scissor{};
	};

	struct glyph
	{
		float advance{};
		float bearing_x{}, bearing_y{};
		float width{}, height{};
		float atlas_x{}, atlas_y{};
	};

	struct font
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> atlas_tex{};
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> atlas_srv{};
		int atlas_w{};
		int atlas_h{};

		float size{};
		float ascent{};
		float descent{};
		float line_height{};

		std::unordered_map<char32_t, glyph> glyph_cache{};
		glyph missing_glyph{};

		font* fallback{};

		void* ft_library{};
		void* ft_face{};

		int pen_x{ 1 };
		int pen_y{ 1 };
		int row_h{ 0 };
		std::vector<std::uint8_t> atlas_bitmap{};
		bool atlas_dirty{};

		~font( );

		[[nodiscard]] const glyph& get( char32_t cp );
		[[nodiscard]] bool has_glyph( char32_t cp ) const;
		[[nodiscard]] std::pair<font*, const glyph*> resolve( char32_t cp );
		[[nodiscard]] std::pair<float, float> measure( std::string_view str );

		void flush_atlas( );
		bool rasterize( char32_t cp );
	};

	struct draw_list
	{
		std::vector<vertex> vertices{};
		std::vector<std::uint32_t> indices{};
		std::vector<draw_cmd> commands{};
		std::vector<D3D11_RECT> clip_stack{};

		void clear( );

		void push_clip( float x, float y, float w, float h );
		void push_clip_absolute( float x, float y, float w, float h );
		void pop_clip( );

		void rect_filled( float x, float y, float w, float h, color col );
		void rect_filled( float x, float y, float w, float h, color col, corner_radius rounding, bool aa = true );
		void rect_filled_gradient( float x, float y, float w, float h, color tl, color tr, color br, color bl );
		void rect_filled_gradient( float x, float y, float w, float h, color tl, color tr, color br, color bl, corner_radius rounding, bool aa = true );
		void rect_filled_blurred( float x, float y, float w, float h, color tint = color{ 255, 255, 255, 255 } );
		void rect_filled_blurred( float x, float y, float w, float h, corner_radius rounding, color tint = color{ 255, 255, 255, 255 }, bool aa = true );
		void circle_filled( float cx, float cy, float radius, color col, int segments = 0, bool aa = true );
		void triangle_filled( float x0, float y0, float x1, float y1, float x2, float y2, color col, bool aa = true );
		void convex_filled( std::span<const float> points, color col, bool aa = true );

		void rect( float x, float y, float w, float h, color col, float thickness = 1.0f, bool aa = true );
		void rect( float x, float y, float w, float h, color col, corner_radius rounding, float thickness = 1.0f, bool aa = true );
		void circle( float cx, float cy, float radius, color col, float thickness = 1.0f, int segments = 0, bool aa = true );
		void line( float x0, float y0, float x1, float y1, color col, float thickness = 1.0f, bool aa = true );
		void polyline( std::span<const float> points, color col, bool closed, float thickness = 1.0f, bool aa = true );
		void polyline_gradient( std::span<const float> points, std::span<const color> colors, bool closed, float thickness = 1.0f, bool aa = true );

		void text( float x, float y, std::string_view str, color col, font* f = nullptr );
		void text( float x, float y, std::string_view str, color col, text_style style, font* f = nullptr );
		void text( float x, float y, std::string_view str, color col, text_style style, color shadow_col, font* f = nullptr );

		void image( float x, float y, float w, float h, ID3D11ShaderResourceView* tex, color tint = color{ 255, 255, 255, 255 } );
		void image( float x, float y, float w, float h, ID3D11ShaderResourceView* tex, corner_radius rounding, color tint = color{ 255, 255, 255, 255 }, bool aa = true );
		void image_uv( float x, float y, float w, float h, ID3D11ShaderResourceView* tex, float u0, float v0, float u1, float v1, color tint = color{ 255, 255, 255, 255 } );

		void ensure_cmd( ID3D11ShaderResourceView* texture );
		std::uint32_t emit_vtx( float x, float y, float u, float v, color c );
		void emit_idx( std::uint32_t a, std::uint32_t b, std::uint32_t c );
		void emit_quad( std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d );

		void build_rounded_rect_path( float x, float y, float w, float h, corner_radius r, std::vector<float>& path ) const;
		[[nodiscard]] int auto_segments( float radius ) const;
	};

	struct gif_image
	{
		struct frame
		{
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv{};
			float delay{};
			int x{}, y{}, w{}, h{};
			int disposal{};
		};

		std::vector<frame> frames{};
		std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> composited{};
		int canvas_w{};
		int canvas_h{};
		int current_idx{};
		float elapsed{};

		void update( float dt );
		void reset( );

		[[nodiscard]] ID3D11ShaderResourceView* current_srv( ) const;
		[[nodiscard]] int frame_count( ) const { return static_cast< int >( frames.size( ) ); }
		[[nodiscard]] int width( ) const { return canvas_w; }
		[[nodiscard]] int height( ) const { return canvas_h; }
		[[nodiscard]] bool valid( ) const { return !frames.empty( ) && !composited.empty( ); }
	};

	bool initialize( ID3D11Device* device, ID3D11DeviceContext* context );

	void begin_frame( bool update_timing = true );
	void end_frame( );

	[[nodiscard]] draw_list& get( layer l = layer::middle );
	[[nodiscard]] draw_list& get_glow( layer l = layer::middle );

	[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> create_srv_from_rgba( const std::uint8_t* pixels, int w, int h );
	[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> load_texture( std::span<const std::byte> data, int* w = nullptr, int* h = nullptr );
	[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> load_svg( std::span<const std::byte> data, float scale = 1.0f, int* out_width = nullptr, int* out_height = nullptr );
	[[nodiscard]] Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> load_svg( const char* svg_text, float scale = 1.0f, int* out_width = nullptr, int* out_height = nullptr );

	[[nodiscard]] gif_image load_gif( std::span<const std::byte> data );
	[[nodiscard]] font* load_font( std::span<const std::byte> data, float size_px, int atlas_w = 1024, int atlas_h = 1024 );

	void push_font( font* f );
	void pop_font( );

	[[nodiscard]] font* current_font( );
	[[nodiscard]] font* primary_font( );

	[[nodiscard]] std::pair<int, int> viewport_size( );
	[[nodiscard]] std::pair<float, float> measure_text( std::string_view str, font* f = nullptr );

	[[nodiscard]] float delta_time( );
	[[nodiscard]] float framerate( );

	[[nodiscard]] ID3D11Device* device( );

} // namespace xdraw

namespace tokens {

	// nexoria — flat neutral charcoal surfaces with a soft periwinkle accent.
	inline xdraw::color col_accent{ 160, 170, 220, 255 };   // #A0AADC
	inline xdraw::color col_accent_dim{ 160, 170, 220, 60 };
	inline xdraw::color col_dark{ 18, 18, 18, 255 };        // #121212  window background
	inline xdraw::color col_text{ 215, 215, 215, 255 };     // #D7D7D7  primary text
	inline xdraw::color col_text_dim{ 140, 140, 140, 255 }; // #8C8C8C  secondary text
	inline xdraw::color col_text_faint{ 100, 100, 100, 255 };// #646464  disabled
	inline xdraw::color col_card{ 14, 14, 14, 255 };        // #0E0E0E  group box surface
	inline xdraw::color col_elevated{ 12, 12, 12, 255 };    // #0C0C0C  header / nested strip
	inline xdraw::color col_surface{ 24, 24, 24, 255 };     // #181818  inputs / controls
	inline xdraw::color col_border{ 45, 45, 45, 255 };      // #2D2D2D  window border
	inline xdraw::color col_border_soft{ 38, 38, 38, 255 }; // #262626  group box border
	inline xdraw::color col_hover{ 38, 38, 38, 255 };
	inline xdraw::color col_pressed{ 22, 22, 22, 255 };

	// Layout constants — flat shell with the navigation moved to the top.
	constexpr auto window_w{ 940.0f };
	constexpr auto window_h{ 540.0f };
	constexpr auto header_h{ 28.0f };
	constexpr auto subtab_bar_h{ 24.0f };
	constexpr auto gap{ 6.0f };
	constexpr auto pad{ 6.0f };

	// Everything is square: the shell reads as a set of stacked instrument panels.
	constexpr auto card_rounding{ 0.0f };
	constexpr auto btn_rounding{ 0.0f };
	constexpr auto pill_rounding{ 999.0f };

	// Group box caption sitting on the top border.
	constexpr auto group_title_h{ 17.0f };
	constexpr auto group_title_pad{ 4.0f };

	// Width of the accent hairline that fades out at both ends.
	constexpr auto accent_line_fade{ 0.15f };

} // namespace tokens

// Flat panel primitives shared by every in-game indicator (keybinds, spectator
// list, doubletap, sound esp, watermark). Kept next to tokens so any HUD code
// can reach them without extra includes, and so all indicators stay in one
// visual language: square corners, opaque surfaces, hairline borders, captions
// punched into the top border and accent hairlines that fade at both ends.
namespace hud {

	constexpr auto caption_h{ 17.0f };
	constexpr auto caption_pad{ 4.0f };
	constexpr auto caption_gap{ 5.0f };

	[[nodiscard]] constexpr std::uint8_t fade_alpha( float a ) noexcept
	{
		return static_cast< std::uint8_t >( 255.0f * ( a < 0.0f ? 0.0f : ( a > 1.0f ? 1.0f : a ) ) );
	}

	// Opaque surface + 1px border, optionally with a borderless variant.
	void surface( xdraw::draw_list& dl, float x, float y, float w, float h, float alpha, bool border );
	void caption( xdraw::draw_list& dl, float x, float y, float w, std::string_view text, float alpha );
	void accent_line( xdraw::draw_list& dl, float x, float y, float w, float thickness, float alpha );

	// Stacked list: caption strip on top, then flat rows separated by hairlines.
	void list_frame( xdraw::draw_list& dl, float x, float y, float w, float h, std::string_view title, float alpha );
	void list_row( xdraw::draw_list& dl, float x, float y, float w, float h, float alpha, bool selected );

} // namespace hud
