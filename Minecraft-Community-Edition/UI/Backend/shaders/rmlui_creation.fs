$input v_color0, v_texcoord0, v_position

/*
 * rmlui_creation.fs — Clip-mask stencil-write fragment shader
 *
 * This shader is used during RenderToClipMask.  It writes nothing to the
 * colour buffer — all the work is done by the stencil operations configured
 * on the CPU side.  We still need a valid fragment shader for bgfx to accept
 * the draw call.
 */

#include <bgfx_shader.sh>

void main()
{
    // Discard nothing — every fragment passes so the stencil op fires.
    // Output transparent black; colour writes are disabled by the CPU state.
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}