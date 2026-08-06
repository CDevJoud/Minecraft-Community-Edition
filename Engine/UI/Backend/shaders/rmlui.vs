$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0, v_position

/*
 * rmlui.vs — Main vertex shader for RmlUI geometry
 *
 * Transforms vertex positions by the combined model-view-projection matrix.
 * Passes through vertex colour (premultiplied alpha) and texture coordinates.
 * Applies bgfx clip-space adjustment for non-OpenGL backends.
 */

#include <bgfx_shader.sh>

uniform mat4 u_transform;

void main() {
	vec4 pos = mul(u_transform, vec4(a_position, 0.0f, 1.0f));
	gl_Position = pos;

	v_color0 = a_color0;
	v_texcoord0 = a_texcoord0;

	#if !BGFX_SHADER_LANGUAGE_GLSL
	// For HLSL, Metal, Vulkan - flip V to match top-left texture origin
    //v_texcoord0.y = 1.0 - v_texcoord0.y;
	#endif

	v_position = a_position;
}
