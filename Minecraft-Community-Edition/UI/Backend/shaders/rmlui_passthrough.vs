$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0, v_position

/*
 * rmlui_passthrough.vs — Passthrough vertex shader for fullscreen quads
 *
 * The quad vertices are in NDC space [-1, 1].
 * UV coordinates are taken from a_texcoord0 and optionally transformed
 * by u_texParams (offset.xy, scale.xy).
 */

#include <bgfx_shader.sh>

uniform vec4 u_texParams;  // xy = uv_offset, zw = uv_scale

void main()
{
    v_position = a_position;
    gl_Position = vec4(a_position, 0.0, 1.0);

    // Transform UV: output = offset + texcoord * scale
    v_texcoord0 = u_texParams.xy + a_texcoord0 * u_texParams.zw;

    #if !BGFX_SHADER_LANGUAGE_GLSL
    // For HLSL, Metal, Vulkan - flip V to match top-left texture origin
    v_texcoord0.y = 1.0 - v_texcoord0.y;
    #endif
    
    v_color0    = a_color0;
}