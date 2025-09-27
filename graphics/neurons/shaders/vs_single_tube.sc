$input a_position
$output v_color0

#include "common.sh"

void main()
{
    // 2 floats (t, phi)
    vec2 tp = a_position.xy;
    // Make a simple circle in XY plane
    float t   = tp.x;   // longitudinal [0..1]
    float phi = tp.y;   // angular [0..2π]

    float radius = 1.0;
    vec3 local = vec3(radius * cos(phi), radius * sin(phi), t * 2.0 - 1.0);

    v_color0 = vec4(1.0, 0.0, 0.0, 1.0);

    gl_Position = mul(u_modelViewProj, vec4(local, 1.0));
}