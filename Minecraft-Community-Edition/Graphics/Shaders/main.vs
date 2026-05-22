//$input a_position, a_color0, a_texcoord0
//$output v_color0, v_texcoord0
//
//#include <bgfx_shader.sh>
//
//void main() {
//	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0f));
//	v_color0 = a_color0;
//	v_texcoord0 = vec2(a_texcoord0.x, 1.0f - a_texcoord0.y);
//}

$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

#include <bgfx_shader.sh>

// Uniforms you can set from CPU side
uniform vec4 u_time;      // x = time
uniform vec4 u_params;    // x = amplitude, y = frequency, z = speed

void main() {
    vec3 pos = a_position;

    float amplitude = u_params.x;
    float frequency = u_params.y;
    float speed = u_params.z;

    // Create sine wave along x-axis affecting y
    pos.y += sin(pos.x * frequency + u_time.x * speed) * amplitude;
    pos.x += sin(pos.x * 0.5 + u_time.x * 0.9f) * 1.0f;

    gl_Position = mul(u_modelViewProj, vec4(pos, 1.0));
    v_color0 = a_color0;
    v_texcoord0 = vec2(a_texcoord0.x, 1.0 - a_texcoord0.y);
}