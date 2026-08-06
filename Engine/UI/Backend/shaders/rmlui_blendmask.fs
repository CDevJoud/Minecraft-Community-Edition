$input v_color0, v_texcoord0, v_position

/*
 * rmlui_blendmask.fs — Blend-mask fragment shader
 *
 * Multiplies the source texture by the mask texture's alpha channel.
 * Used for CSS mask-image filter.
 *
 * Samplers:
 *   s_texture0 = source (the content being masked)
 *   s_texture1 = mask   (only .a channel is used)
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);
SAMPLER2D(s_texture1, 1);

void main()
{
    vec4 src  = texture2D(s_texture0, v_texcoord0);
    vec4 mask = texture2D(s_texture1, v_texcoord0);

    // Modulate source by mask alpha
    gl_FragColor = src * mask.a;
}