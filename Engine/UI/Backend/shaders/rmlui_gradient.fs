$input v_color0, v_texcoord0, v_position

/*
 * rmlui_gradient.fs — Gradient decorator fragment shader
 *
 * Supports linear, radial, conic, and their repeating variants.
 * Colour stops are baked into a 1D look-up texture (s_texture0).
 *
 * Uniforms:
 *   u_gradientParams.x = gradient function:
 *       0 = linear
 *       1 = radial
 *       2 = conic
 *       3 = repeating-linear
 *       4 = repeating-radial
 *       5 = repeating-conic
 *   u_gradientP.xy = point P (start for linear, centre for radial/conic)
 *   u_gradientP.zw = point Q (end for linear, edge for radial)
 */

#include <bgfx_shader.sh>

SAMPLER2D(s_texture0, 0);  // 1D colour-stop look-up texture

uniform vec4 u_gradientParams;  // x = function, y/z/w = unused
uniform vec4 u_gradientP;       // xy = P, zw = Q

void main()
{
    int func = int(u_gradientParams.x);

    vec2 P = u_gradientP.xy;
    vec2 Q = u_gradientP.zw;
    vec2 pos = v_position;  // screen-space position from vertex shader

    float t = 0.0;

    // Linear gradient
    if (func == 0 || func == 3)
    {
        vec2 dir = Q - P;
        float len2 = dot(dir, dir);
        if (len2 > 0.0001)
            t = dot(pos - P, dir) / len2;
        else
            t = 0.0;
    }
    // Radial gradient 
    else if (func == 1 || func == 4)
    {
        vec2 diff = Q - P;
        float radius = length(diff);
        if (radius > 0.0001)
            t = length(pos - P) / radius;
        else
            t = 0.0;
    }
    // Conic gradient 
    else if (func == 2 || func == 5)
    {
        vec2 d = pos - P;
        // Angle from the direction P->Q, normalised to [0, 1]
        vec2 refDir = normalize(Q - P);
        float angle = atan2(d.y, d.x) - atan2(refDir.y, refDir.x);
        // Wrap to [0, 2pi]
        const float PI  = 3.14159265;
        const float TAU = 6.28318530;
        if (angle < 0.0) angle += TAU;
        t = angle / TAU;
    }

    // Repeating variants
    if (func >= 3)
    {
        t = fract(t);
    }
    else
    {
        t = clamp(t, 0.0, 1.0);
    }

    // Look up colour from the 1D stop texture
    vec4 color = texture2D(s_texture0, vec2(t, 0.5));

    // Premultiply alpha (stop texture stores straight alpha)
    color.rgb *= color.a;

    // Modulate by vertex colour
    gl_FragColor = color * v_color0;
}