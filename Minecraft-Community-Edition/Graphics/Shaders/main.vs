//$input a_position, a_color0, a_texcoord0
//$output v_color0, v_texcoord0
//
//#include <bgfx_shader.sh>
//
//void main() {
//	gl_Position = mul(u_modelViewProj, vec4(a_position.xyz, 1.0));
//	v_color0 = a_color0;
//	v_texcoord0 = vec2(a_texcoord0.x, 1.0f - a_texcoord0.y);
//}


$input a_position, a_color0, a_texcoord0


void main() {
	gl_Position = vec4(a_position, 1.0f);
}