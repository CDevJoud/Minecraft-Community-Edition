$input v_color0, v_texcoord0, v_position

/*
 * rmlui_colormatrix.fs — Colour matrix filter fragment shader
 *
 * Applies a 4×4 colour matrix plus a 4-component translate to each texel.
 * Used for brightness, contrast, grayscale, invert, opacity, saturate,
 * sepia, and hue-rotate CSS filters.
 *
 * Uniforms:
 *   u_colorMatrix    = 4×4 colour transform matrix
 *   u_colorTranslate = 4-component additive offset
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);

uniform mat4 u_colorMatrix;
uniform vec4 u_colorTranslate;

void main()
{
    vec4 texColor = texture2D(s_texture0, v_texcoord0);

    // Un-premultiply alpha before applying the colour matrix
    // (colour matrix operates in straight-alpha space)
    if (texColor.a > 0.001)
        texColor.rgb /= texColor.a;

    // Apply the 4×4 matrix + translate
    vec4 result = mul(u_colorMatrix, texColor) + u_colorTranslate;

    // Clamp and re-premultiply
    result = clamp(result, 0.0, 1.0);
    result.rgb *= result.a;

    gl_FragColor = result;
}