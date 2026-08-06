$input v_color0, v_texcoord0, v_position

/*
 * rmlui_texture.fs — Textured fragment shader
 *
 * Samples the texture and multiplies by the vertex colour.
 * Both texture and vertex colour are expected in premultiplied-alpha format.
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);

void main()
{
    vec4 texColor = texture2D(s_texture0, v_texcoord0);
    gl_FragColor  = texColor * v_color0;
}