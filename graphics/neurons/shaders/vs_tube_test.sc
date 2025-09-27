$input a_position, i_data0
$output v_color0

#include "common.sh"

void main()
{
    // Our static grid provides 2 floats (t, phi)
    vec2 tp = a_position.xy;

    // Instance data
    vec3 p0 = i_data0.xyz;
    float radius = i_data0.w;

    // Make a simple circle in XY plane
    float t   = tp.x;   // longitudinal [0..1]
    float phi = tp.y;   // angular [0..2π]

    vec3 local = vec3(radius * cos(phi), radius * sin(phi), t * 2.0 - 1.0);

    // Place it at instance origin
    vec3 worldPos = p0 + local;

    gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));

    v_color0 = vec4(1.0, 0.0, 0.0, 1.0); // bright red
}