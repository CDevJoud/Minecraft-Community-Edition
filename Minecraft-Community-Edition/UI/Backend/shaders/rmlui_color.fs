$input v_color0, v_texcoord0, v_position

/*
 * rmlui_color.fs — Vertex-colour-only fragment shader
 *
 * Outputs the interpolated vertex colour directly.
 * Used for untextured RmlUI geometry.
 */

#include <bgfx_shader.sh>

void main()
{
    gl_FragColor = v_color0;
}