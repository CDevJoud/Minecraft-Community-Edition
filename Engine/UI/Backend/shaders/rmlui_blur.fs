$input v_color0, v_texcoord0, v_position

/*
 * rmlui_blur.fs — Separable Gaussian blur fragment shader
 *
 * Performs a single-direction Gaussian blur pass.
 * Two passes (horizontal + vertical) produce a full 2D blur.
 * Matches the GL3 RmlUI implementation weight calculation exactly.
 *
 * Uniforms:
 *   u_blurParams.x  = sigma
 *   u_blurParams.yz = direction (1,0) for horizontal, (0,1) for vertical
 *   u_texelSize.xy  = 1.0 / textureSize
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);

uniform vec4 u_blurParams;  // x=sigma, y=dir_x, z=dir_y, w=unused
uniform vec4 u_texelSize;   // x=1/w, y=1/h, z=unused, w=unused

#define NUM_WEIGHTS 8

void main()
{
    float sigma = u_blurParams.x;
    vec2  dir   = u_blurParams.yz;
    vec2  texel = u_texelSize.xy;

    // Compute Gaussian weights on the fly (matching GL3 exactly)
    float weights[NUM_WEIGHTS];
    float wsum = 0.0;
    for (int i = 0; i < NUM_WEIGHTS; i++)
    {
        float x = float(i);
        weights[i] = exp(-0.5 * (x * x) / (sigma * sigma));
        wsum += (i == 0) ? weights[i] : 2.0 * weights[i];
    }
    for (int i = 0; i < NUM_WEIGHTS; i++)
        weights[i] /= wsum;

    // Sample: centre + symmetric taps
    vec2 step = dir * texel;
    vec4 color = texture2D(s_texture0, v_texcoord0) * weights[0];

    for (int i = 1; i < NUM_WEIGHTS; i++)
    {
        vec2 offset = step * float(i);
        color += texture2D(s_texture0, v_texcoord0 + offset) * weights[i];
        color += texture2D(s_texture0, v_texcoord0 - offset) * weights[i];
    }

    gl_FragColor = color;
}