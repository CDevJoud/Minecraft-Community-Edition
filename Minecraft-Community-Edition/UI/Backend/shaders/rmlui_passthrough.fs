$input v_color0, v_texcoord0, v_position

/*
 * rmlui_passthrough.fs — Simple texture passthrough
 *
 * Used for blitting layers and compositing framebuffers.
 * Outputs the sampled texel colour without modification.
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);

void main()
{
    gl_FragColor = texture2D(s_texture0, v_texcoord0);
}