$input v_color0, v_texcoord0, v_position

/*
 * rmlui_dropshadow.fs — Drop-shadow fragment shader
 *
 * Takes the source layer alpha, offsets the UV, and outputs a solid colour
 * multiplied by that alpha. The result is later blurred by the blur shader.
 *
 * Uniforms:
 *   u_shadowExtra.xy = offset in UV space (offset / textureSize)
 *   u_shadowColor    = shadow colour (premultiplied alpha)
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);

uniform vec4 u_shadowExtra;  // xy = UV offset, zw = unused
uniform vec4 u_shadowColor;  // rgba shadow colour

void main()
{
    // Sample at the offset position to shift the shadow
    vec2 uv = v_texcoord0 - u_shadowExtra.xy;
    vec4 texColor = texture2D(s_texture0, uv);

    // Use source alpha to modulate shadow colour
    float alpha = texColor.a;
    gl_FragColor = u_shadowColor * alpha;
}