#include <pch/pch.hpp>
#include <utilities/memory/memory.hpp>
#include <utilities/addresses/addresses.hpp>
#include <utilities/logging/logging.hpp>
#include <protection/game_addresses.hpp>
#include "../systems.hpp"

namespace systems {

	namespace detail {

		// "metallic" = chrome that reflects the baked environment cubemap of the surrounding
		// scene (sky, walls, ambient) via g_flMetalness + the flat mirror normal
		// (default_normal_1b833b2a). F_PAINT_VERTEX_COLORS lets the menu colour tint apply.
		static constexpr char metallic[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"

	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_DISABLE_Z_PREPASS = 1
	F_DISABLE_Z_WRITE = 1
	F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

	g_vColorTint = [ 1.0, 1.0, 1.0, 1.0 ]
	g_bFogEnabled = 0
	g_flMetalness = 1.000
	g_flModelTintAmount = 1.000
	g_nScaleTexCoordUByModelScaleAxis = 0
	g_nScaleTexCoordVByModelScaleAxis = 0
	g_nTextureAddressModeU = 0
	g_nTextureAddressModeV = 0
	g_flTexCoordRotation = 0.000

	g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
	g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
})#";

		// Occluded twin: identical to the visible chrome but with depth buffering
		// disabled so it draws through walls.
		static constexpr char metallic_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
{
	shader = "csgo_complex.vfx"

	F_PAINT_VERTEX_COLORS = 1
	F_TRANSLUCENT = 1
	F_DISABLE_Z_BUFFERING = 1
	F_DISABLE_Z_PREPASS = 1
	F_DISABLE_Z_WRITE = 1
	F_BLEND_MODE = 1
	F_RENDER_BACKFACES = 0

	g_vColorTint = [ 1.0, 1.0, 1.0, 1.0 ]
	g_bFogEnabled = 0
	g_flMetalness = 1.000
	g_flModelTintAmount = 1.000
	g_nScaleTexCoordUByModelScaleAxis = 0
	g_nScaleTexCoordVByModelScaleAxis = 0
	g_nTextureAddressModeU = 0
	g_nTextureAddressModeV = 0
	g_flTexCoordRotation = 0.000

	g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
	g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
	g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
})#";

		static constexpr char matte[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "generic.vfx"

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
            })#";

		// TODO
		static constexpr char matte_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_character.vfx"

                F_PAINT_VERTEX_COLORS = 1
                F_DISABLE_Z_BUFFERING = 1
                F_DISABLE_Z_PREPASS = 1
                F_DISABLE_Z_WRITE = 1
                F_TRANSLUCENT = 1
                F_BLEND_MODE = 1

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_bFogEnabled = 0
                g_flMetalness = 0.350
                g_flRoughness = 0.450

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
                g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
            })#";

		static constexpr char flat[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_unlitgeneric.vfx"

                F_RENDER_BACKFACES = 0
                F_DISABLE_Z_BUFFERING = 0
                F_PAINT_VERTEX_COLORS = 1
                F_TRANSLUCENT = 1
                F_BLEND_MODE = 1

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
            })#";

		static constexpr char flat_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_character.vfx"

                F_PAINT_VERTEX_COLORS = 1
                F_DISABLE_Z_BUFFERING = 1
                F_DISABLE_Z_PREPASS = 1
                F_DISABLE_Z_WRITE = 1
                F_TRANSLUCENT = 1
                F_BLEND_MODE = 1

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_bFogEnabled = 0
                g_flMetalness = 0.350
                g_flRoughness = 0.450

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_79a2e0d0.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_1b833b2a.vtex"
                g_tMetalness = resource:"materials/default/default_metal_tga_8fbc2820.vtex"
            })#";

		static constexpr char bloom[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "solidcolor.vfx"

                F_DISABLE_Z_WRITE = 0

                g_vColorTint = [8.0, 8.0, 8.0]
            })#";

		static constexpr char bloom_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "solidcolor.vfx"

                F_IGNOREZ = 1
                F_DISABLE_Z_WRITE = 1

                g_vColorTint = [5.0, 5.0, 5.0]
            })#";

		static constexpr char outlines[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1

                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
                g_flOpacityScale = 0.45
                g_flFresnelExponent = 0.75
                g_flFresnelFalloff = 1.0
                g_flFresnelMax = 0.0
                g_flFresnelMin = 1.0
				g_flColorBoost = 2.25
                g_flToolsVisCubemapReflectionRoughness = 1.0
                g_flBeginMixingRoughness = 1.0

                g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"
            })#";

		static constexpr char outlines_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_DISABLE_Z_BUFFERING = 1
                F_DISABLE_Z_WRITE = 1

                g_vColorTint = [1.0, 1.0, 1.0, 0.0]
                g_flOpacityScale = 0.45
                g_flFresnelExponent = 0.75
                g_flFresnelFalloff = 1.0
                g_flFresnelMax = 0.0
                g_flFresnelMin = 1.0
				g_flColorBoost = 2.25
                g_flToolsVisCubemapReflectionRoughness = 1.0
                g_flBeginMixingRoughness = 1.0

                g_tColor = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_fde710a5.vtex"
                g_tSceneDepth = resource:"materials/default/default_mask_tga_fde710a5.vtex"
            })#";

		static constexpr char glow[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
				g_flFresnelExponent = 1.5
				g_flFresnelFalloff = 5.0
				g_flFresnelMax = 0.0
				g_flFresnelMin = 1.0
				g_flColorBoost = 20.0
				g_flOpacityScale = 0.6

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
            })#";

		static constexpr char glow_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 1
                F_DISABLE_Z_BUFFERING = 1
                F_DISABLE_Z_WRITE = 1
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
				g_flFresnelExponent = 1.5
				g_flFresnelFalloff = 5.0
				g_flFresnelMax = 0.0
				g_flFresnelMin = 1.0
				g_flColorBoost = 20.0
				g_flOpacityScale = 0.6

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
            })#";

		static constexpr char glow2[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.00000, 1.00000, 1.00000]
				g_flColorBoost = 20
				g_flOpacityScale = 0.6999999
				g_flFresnelExponent = 10
				g_flFresnelFalloff = 10
				g_flFresnelMax = 0
				g_flFresnelMin = 1

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
            })#";

		static constexpr char glow2_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_IGNOREZ = 1
                F_DISABLE_Z_BUFFERING = 1
                F_DISABLE_Z_WRITE = 1
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.00000, 1.00000, 1.00000]
				g_flColorBoost = 20
				g_flOpacityScale = 0.6999999
				g_flFresnelExponent = 10
				g_flFresnelFalloff = 10
				g_flFresnelMax = 0
				g_flFresnelMin = 1

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
            })#";

		static constexpr char flat2[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 0
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_PAINT_VERTEX_COLORS = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_bFogEnabled = 0
                g_flColorBoost = 0
                g_flOpacityScale = 1.0

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
            })#";

		static constexpr char flat2_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_effects.vfx"

                F_ADDITIVE_BLEND = 0
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_PAINT_VERTEX_COLORS = 1
                F_IGNOREZ = 1
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 1
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_bFogEnabled = 0
                g_flColorBoost = 0
                g_flOpacityScale = 1.0

                g_tColor = resource:"materials/dev/primary_white_color_tga_21186c76.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
            })#";

		static constexpr char flow[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_unlitgeneric.vfx"

                F_TRANSLUCENT = 1
                F_ADDITIVE_BLEND = 1
                F_NO_CULLING = 1
                F_UNLIT = 1
                F_PAINT_VERTEX_COLORS = 1

                F_DISABLE_Z_PREPASS = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0

                g_tColor = resource:"materials/dev/water_waves.vtex"

                g_vTexCoordScrollSpeed = [0.5, 0.0]

                g_flFresnelExponent = 1.0
                g_flFresnelFalloff = 1.0
                g_flFresnelMax = 1.0

                g_vOverrideColor = [1.0, 1.0, 1.0, 1.0]
            })#";

		static constexpr char flow_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_unlitgeneric.vfx"

                F_TRANSLUCENT = 1
                F_ADDITIVE_BLEND = 1
                F_NO_CULLING = 1
                F_UNLIT = 1
                F_PAINT_VERTEX_COLORS = 1

                F_DISABLE_Z_PREPASS = 1
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 1

                g_tColor = resource:"materials/dev/water_waves.vtex"

                g_vTexCoordScrollSpeed = [0.5, 0.0]

                g_flFresnelExponent = 1.0
                g_flFresnelFalloff = 1.0
                g_flFresnelMax = 1.0

                g_vOverrideColor = [1.0, 1.0, 1.0, 1.0]
            })#";

		static constexpr char darkmatter[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_unlitgeneric.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_PAINT_VERTEX_COLORS = 1
                F_IGNOREZ = 0
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 0
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_vTexCoordScrollSpeed = [0.13, 0.13]

                g_tColor = resource:"materials/dev/water_waves.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
            })#";

		static constexpr char darkmatter_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_unlitgeneric.vfx"

                F_ADDITIVE_BLEND = 1
                F_BLEND_MODE = 1
                F_TRANSLUCENT = 1
                F_PAINT_VERTEX_COLORS = 1
                F_IGNOREZ = 1
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 1
                F_RENDER_BACKFACES = 0

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_vTexCoordScrollSpeed = [0.13, 0.13]

                g_tColor = resource:"materials/dev/water_waves.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7652cb.vtex"
                g_tMask1 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask2 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tMask3 = resource:"materials/default/default_mask_tga_344101f8.vtex"
                g_tRoughness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tMetalness = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
                g_tAmbientOcclusion = resource:"materials/default/default_normal_tga_b3f4ec4c.vtex"
            })#";

		static constexpr char data[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_complex.vfx"

                F_DISABLE_Z_PREPASS = 0
                F_DISABLE_Z_WRITE = 0
                F_DISABLE_Z_BUFFERING = 0

                F_TRANSLUCENT = 1
                F_SELF_ILLUM = 1
                F_PAINT_VERTEX_COLORS = 1

                g_bFogEnabled = 1

                g_flModelTintAmount = 1

                g_flSelfIllumBrightness = 1.2
                g_flSelfIllumScale = 2

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_vSelfIllumTint = [1.0, 1.0, 1.0, 1.0]

                g_vTexCoordScale = [10.0, 2.0]
                g_vTexCoordOffset = [0.0, 0.0]
                g_vTexCoordScrollSpeed = [0.2, 0.2]

                g_tColor = resource:"materials/default/default_color_tga_71e37c58.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7be61377.vtex"

                g_tSelfIllumMask = resource:"materials/default/stickers/squares_glitter_normal_tga_25145674.vtex"

                g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
            })#";

		static constexpr char data_ignorez[ ] = R"#(<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d}
            format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->
            {
                shader = "csgo_complex.vfx"

                F_DISABLE_Z_PREPASS = 1
                F_DISABLE_Z_WRITE = 1
                F_DISABLE_Z_BUFFERING = 1

                F_TRANSLUCENT = 1
                F_SELF_ILLUM = 1
                F_PAINT_VERTEX_COLORS = 1

                g_bFogEnabled = 1

                g_flModelTintAmount = 1

                g_flSelfIllumBrightness = 1.2
                g_flSelfIllumScale = 2

                g_vColorTint = [1.0, 1.0, 1.0, 1.0]
                g_vSelfIllumTint = [1.0, 1.0, 1.0, 1.0]

                g_vTexCoordScale = [10.0, 2.0]
                g_vTexCoordOffset = [0.0, 0.0]
                g_vTexCoordScrollSpeed = [0.2, 0.2]

                g_tColor = resource:"materials/default/default_color_tga_71e37c58.vtex"
                g_tNormal = resource:"materials/default/default_normal_tga_7be61377.vtex"

                g_tSelfIllumMask = resource:"materials/default/stickers/squares_glitter_normal_tga_25145674.vtex"

                g_tAmbientOcclusion = resource:"materials/default/default_ao_tga_559f1ac6.vtex"
            })#";

	} // namespace detail

	bool materials::initialize( )
	{
		const auto metallic_ignorez_ptr = load( detail::metallic_ignorez, xs( "materials/dev/metallic_ignorez.vmat" ) );
		const auto matte_ignorez_ptr = load( detail::matte_ignorez, xs( "materials/dev/matte_ignorez.vmat" ) );
		const auto flat_ignorez_ptr = load( detail::flat_ignorez, xs( "materials/dev/flat_ignorez.vmat" ) );
		const auto bloom_ignorez_ptr = load( detail::bloom_ignorez, xs( "materials/dev/bloom_ignorez.vmat" ) );
		const auto outlines_ignorez_ptr = load( detail::outlines_ignorez, xs( "materials/dev/outlines_ignorez.vmat" ) );
		const auto glow_ignorez_ptr = load( detail::glow_ignorez, xs( "materials/dev/glow_ignorez.vmat" ) );
		const auto glow2_ignorez_ptr = load( detail::glow2_ignorez, xs( "materials/dev/glow2_ignorez.vmat" ) );
		const auto flat2_ignorez_ptr = load( detail::flat2_ignorez, xs( "materials/dev/flat2_ignorez.vmat" ) );
		const auto flow_ignorez_ptr = load( detail::flow_ignorez, xs( "materials/dev/flow_ignorez.vmat" ) );
		const auto darkmatter_ignorez_ptr = load( detail::darkmatter_ignorez, xs( "materials/dev/darkmatter_ignorez.vmat" ) );
		const auto data_ignorez_ptr = load( detail::data_ignorez, xs( "materials/dev/data_ignorez.vmat" ) );

		const auto metallic_ptr = load( detail::metallic, xs( "materials/dev/metallic.vmat" ) );
		const auto matte_ptr = load( detail::matte, xs( "materials/dev/matte.vmat" ) );
		const auto flat_ptr = load( detail::flat, xs( "materials/dev/flat.vmat" ) );
		const auto bloom_ptr = load( detail::bloom, xs( "materials/dev/bloom.vmat" ) );
		const auto outlines_ptr = load( detail::outlines, xs( "materials/dev/outlines.vmat" ) );
		const auto glow_ptr = load( detail::glow, xs( "materials/dev/glow.vmat" ) );
		const auto glow2_ptr = load( detail::glow2, xs( "materials/dev/glow2.vmat" ) );
		const auto flat2_ptr = load( detail::flat2, xs( "materials/dev/flat2.vmat" ) );
		const auto flow_ptr = load( detail::flow, xs( "materials/dev/flow.vmat" ) );
		const auto darkmatter_ptr = load( detail::darkmatter, xs( "materials/dev/darkmatter.vmat" ) );
		const auto data_ptr = load( detail::data, xs( "materials/dev/data.vmat" ) );

		if ( !metallic_ptr || !matte_ptr || !flat_ptr || !bloom_ptr || !outlines_ptr || !glow_ptr || !glow2_ptr || !flat2_ptr || !flow_ptr || !darkmatter_ptr || !data_ptr || !metallic_ignorez_ptr || !matte_ignorez_ptr || !flat_ignorez_ptr || !bloom_ignorez_ptr || !outlines_ignorez_ptr || !glow_ignorez_ptr || !glow2_ignorez_ptr || !flat2_ignorez_ptr || !flow_ignorez_ptr || !darkmatter_ignorez_ptr || !data_ignorez_ptr )
		{
			return false;
		}

		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::metallic ) ] = metallic_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::matte ) ] = matte_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::flat ) ] = flat_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::bloom ) ] = bloom_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::outlines ) ] = outlines_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::glow ) ] = glow_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::glow2 ) ] = glow2_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::flat2 ) ] = flat2_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::flow ) ] = flow_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::darkmatter ) ] = darkmatter_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::data ) ] = data_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::metallic_ignorez ) ] = metallic_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::matte_ignorez ) ] = matte_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::flat_ignorez ) ] = flat_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::bloom_ignorez ) ] = bloom_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::outlines_ignorez ) ] = outlines_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::glow_ignorez ) ] = glow_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::glow2_ignorez ) ] = glow2_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::flat2_ignorez ) ] = flat2_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::flow_ignorez ) ] = flow_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::darkmatter_ignorez ) ] = darkmatter_ignorez_ptr;
		m_loaded[ static_cast< std::size_t >( settings::esp::cham_ids::data_ignorez ) ] = data_ignorez_ptr;

		return true;
	}

	std::uintptr_t materials::find( settings::esp::cham_ids id )
	{
		const auto index = static_cast< std::size_t >( id );
		if ( index >= m_loaded.size( ) )
		{
			return 0;
		}

		return m_loaded[ index ];
	}

	const char* materials::get_texture_path( std::uintptr_t entry )
	{
		const auto handle = *reinterpret_cast< const std::uintptr_t* >( entry + 0x10 );
		if ( !handle )
		{
			return nullptr;
		}

		const auto resource = *reinterpret_cast< const std::uintptr_t* >( handle + 0x08 );
		if ( !resource )
		{
			return nullptr;
		}

		return *reinterpret_cast< const char** >( resource );
	}

	std::string materials::emit_translucent_kv( std::uintptr_t src_mat )
	{
		const auto kv_count = *reinterpret_cast< const int* >( src_mat + 0x18 );
		const auto kv_array = *reinterpret_cast< const std::uintptr_t* >( src_mat + 0x20 );

		std::string kv =
			"<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} "
			"format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->\n"
			"{\n"
			"shader = \"csgo_complex.vfx\"\n"
			"F_TRANSLUCENT = 1\n"
			"F_ALPHA_TEST = 0\n"
			"F_ADDITIVE_BLEND = 0\n";

		auto should_skip = [ ]( const char* name ) -> bool
			{
				if ( std::strcmp( name, "shader" ) == 0 )
				{
					return true;
				}

				if ( std::strncmp( name, "F_", 2 ) == 0 )
				{
					return true;
				}

				return false;
			};

		for ( auto i = 0; i < kv_count; ++i )
		{
			const auto entry = kv_array + static_cast< std::uintptr_t >( i ) * 0x40;
			const auto name = *reinterpret_cast< const char** >( entry + 0x28 );

			if ( !name || should_skip( name ) )
			{
				continue;
			}

			if ( *reinterpret_cast< const std::uintptr_t* >( entry + 0x10 ) )
			{
				const auto path = get_texture_path( entry );
				if ( path && path[ 0 ] )
				{
					kv += std::format( "{} = resource:\"{}\"\n", name, path );
				}

				continue;
			}

			if ( std::strncmp( name, "g_b", 3 ) == 0 )
			{
				const auto v = static_cast< int >( *reinterpret_cast< const float* >( entry ) );
				kv += std::format( "{} = {}\n", name, v ? 1 : 0 );
				continue;
			}

			if ( std::strncmp( name, "g_n", 3 ) == 0 )
			{
				const auto v = static_cast< int >( *reinterpret_cast< const float* >( entry ) );
				kv += std::format( "{} = {}\n", name, v );
				continue;
			}

			if ( std::strncmp( name, "g_fl", 4 ) == 0 || std::strncmp( name, "g_f", 3 ) == 0 )
			{
				const auto v = *reinterpret_cast< const float* >( entry );
				kv += std::format( "{} = {}\n", name, v );
				continue;
			}

			if ( std::strncmp( name, "g_v", 3 ) == 0 )
			{
				const auto x = *reinterpret_cast< const float* >( entry );
				const auto y = *reinterpret_cast< const float* >( entry + 0x04 );
				const auto z = *reinterpret_cast< const float* >( entry + 0x08 );
				kv += std::format( "{} = [{}, {}, {}]\n", name, x, y, z );
				continue;
			}
		}

		kv += "}\n";
		return kv;
	}

	std::string materials::emit_ignorez_kv( std::uintptr_t src_mat )
	{
		const auto kv_count = *reinterpret_cast< const int* >( src_mat + 0x18 );
		const auto kv_array = *reinterpret_cast< const std::uintptr_t* >( src_mat + 0x20 );

		std::string kv =
			"<!-- kv3 encoding:text:version{e21c7f3c-8a33-41c5-9977-a76d3a32aa0d} "
			"format:generic:version{7412167c-06e9-4698-aff2-e63eb59037e7} -->\n"
			"{\n"
			"shader = \"csgo_complex.vfx\"\n"
			"F_TRANSLUCENT = 1\n"
			"F_DISABLE_Z_BUFFERING = 1\n"
			"F_ALPHA_TEST = 0\n"
			"F_ADDITIVE_BLEND = 0\n";

		auto should_skip = [ ]( const char* name ) -> bool
			{
				if ( std::strcmp( name, "shader" ) == 0 )
				{
					return true;
				}

				if ( std::strncmp( name, "F_", 2 ) == 0 )
				{
					return true;
				}

				return false;
			};

		for ( auto i = 0; i < kv_count; ++i )
		{
			const auto entry = kv_array + static_cast< std::uintptr_t >( i ) * 0x40;
			const auto name = *reinterpret_cast< const char** >( entry + 0x28 );

			if ( !name || should_skip( name ) )
			{
				continue;
			}

			if ( *reinterpret_cast< const std::uintptr_t* >( entry + 0x10 ) )
			{
				const auto path = get_texture_path( entry );
				if ( path && path[ 0 ] )
				{
					kv += std::format( "{} = resource:\"{}\"\n", name, path );
				}

				continue;
			}

			if ( std::strncmp( name, "g_b", 3 ) == 0 )
			{
				const auto v = static_cast< int >( *reinterpret_cast< const float* >( entry ) );
				kv += std::format( "{} = {}\n", name, v ? 1 : 0 );
				continue;
			}

			if ( std::strncmp( name, "g_n", 3 ) == 0 )
			{
				const auto v = static_cast< int >( *reinterpret_cast< const float* >( entry ) );
				kv += std::format( "{} = {}\n", name, v );
				continue;
			}

			if ( std::strncmp( name, "g_fl", 4 ) == 0 || std::strncmp( name, "g_f", 3 ) == 0 )
			{
				const auto v = *reinterpret_cast< const float* >( entry );
				kv += std::format( "{} = {}\n", name, v );
				continue;
			}

			if ( std::strncmp( name, "g_v", 3 ) == 0 )
			{
				const auto x = *reinterpret_cast< const float* >( entry );
				const auto y = *reinterpret_cast< const float* >( entry + 0x04 );
				const auto z = *reinterpret_cast< const float* >( entry + 0x08 );
				kv += std::format( "{} = [{}, {}, {}]\n", name, x, y, z );
				continue;
			}
		}

		kv += "}\n";
		return kv;
	}

	std::uintptr_t materials::load( const char* vmat_data, const char* name )
	{
		constexpr auto kv3_id = cstypes::kv3_id{ "generic", 0x41B818518343427E, 0xB5F447C23C0CDF8C };

		if ( !vmat_data || !name )
		{
			return 0;
		}

		const auto kv3_set_type = PATTERN( patterns::kv3_alloc );
		const auto kv3_destroy = PATTERN( patterns::kv3_destroy );
		const auto kv3_load = MODULE_EXPORT( "tier0.dll:?LoadKV3@@YA_NPEAVKeyValues3@@PEAVCUtlString@@PEBDAEBUKV3ID_t@@2I@Z" );
		const auto material_create = PATTERN( patterns::material_create );
		if ( !kv3_set_type || !kv3_destroy || !kv3_load || !material_create )
		{
			return 0;
		}

		cstypes::key_values3 kv3{};
		if ( memory::call<cstypes::key_values3*>( kv3_set_type, &kv3, 1u, 6u ) != &kv3 )
		{
			return 0;
		}

		cstypes::strong_handle handle{};
		const auto loaded = memory::call<bool>( kv3_load, &kv3, nullptr, vmat_data, &kv3_id, nullptr, 0u );
		if ( loaded )
		{
			memory::call<void*>( material_create, nullptr, &handle, name, &kv3, 0, true );
		}

		// CreateMaterial copies the parsed tree; release the parser-owned value.
		memory::call<void>( kv3_destroy, &kv3, 0u );

		if ( !loaded || !handle.binding )
		{
			return 0;
		}

		const auto material = *reinterpret_cast< const std::uintptr_t* >( handle.binding );
		if ( material )
		{
			std::scoped_lock lock( m_mtx );
			m_handles.push_back( handle );
		}

		return material;
	}

	std::uintptr_t materials::get_or_create_clone( std::uintptr_t src_mat, clone_type type )
	{
		const auto key = src_mat ^ ( static_cast< std::uint64_t >( type ) << 48 );

		{
			std::scoped_lock lock( m_mtx );
			const auto it = m_map.find( key );
			if ( it != m_map.end( ) )
			{
				return it->second;
			}
		}

		const auto kv = type == clone_type::ignorez ? emit_ignorez_kv( src_mat ) : emit_translucent_kv( src_mat );
		const auto prefix = type == clone_type::ignorez ? "_ignorez" : "_clone";
		const auto name = std::format( "materials/{}_{:x}.vmat", prefix, src_mat );
		const auto mat = load( kv.c_str( ), name.c_str( ) );

		{
			std::scoped_lock lock( m_mtx );
			m_map.emplace( key, mat );
		}

		return mat;
	}

	void materials::clear_clones( )
	{
		std::scoped_lock lock( m_mtx );
		m_map.clear( );
	}

	void materials::set_material_vec3( std::uintptr_t mat, const char* param_name, float x, float y, float z )
	{
		const auto kv_count = *reinterpret_cast< const int* >( mat + 0x18 );
		const auto kv_array = *reinterpret_cast< const std::uintptr_t* >( mat + 0x20 );

		for ( auto i = 0; i < kv_count; ++i )
		{
			const auto entry = kv_array + static_cast< std::uintptr_t >( i ) * 0x40;
			const auto name = *reinterpret_cast< const char** >( entry + 0x28 );

			if ( !name || std::strcmp( name, param_name ) != 0 )
			{
				continue;
			}

			*reinterpret_cast< float* >( entry + 0x00 ) = x;
			*reinterpret_cast< float* >( entry + 0x04 ) = y;
			*reinterpret_cast< float* >( entry + 0x08 ) = z;
			return;
		}
	}

} // namespace systems
