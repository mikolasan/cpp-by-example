$input a_position, i_data0, i_data1
$output v_color0

#include "common.sh"

void main()
{
    vec3 pre  = i_data0.xyz;
    vec3 post = i_data1.xyz;

    // Interpolate between pre and post along quad’s x axis (0..1)
    float u = a_position.x;  // horizontal axis goes from pre→post
    float v = a_position.y;  // vertical axis (not used much, just keep quad visible)

    vec3 pos = mix(pre, post, u) + vec3(0.0, v*0.05, 0.0); // tiny vertical offset

    gl_Position = mul(u_viewProj, vec4(pos, 1.0));

    // Color diagnostic:
    // u=0 (pre) = pink, u=1 (post) = light blue, middle = green
    vec3 pink = vec3(1.0, 0.0, 1.0);
    vec3 blue = vec3(0.5, 0.7, 1.0);
    vec3 green= vec3(0.0, 1.0, 0.0);
    v_color0 = vec4(mix(pink, green, u*2.0<=1.0?u*2.0:1.0), 1.0); // 0→pink→green
    if (u>0.5) {
        v_color0.rgb = mix(green, blue, (u-0.5)*2.0);
    }
}

// $input  a_position, i_data0, i_data1, i_data2, i_data3
// $output v_color0

// #include "common.sh"

// void bezier(in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3, float t,
//             out vec3 P, out vec3 T)
// {
//     float u = 1.0 - t;
//     float uu = u*u, tt = t*t;

//     // position
//     P = p0*(uu*u) + p1*(3.0*uu*t) + p2*(3.0*u*tt) + p3*(tt*t);

//     // derivative (tangent, not normalized)
//     T = (p1 - p0)*(3.0*uu) + (p2 - p1)*(6.0*u*t) + (p3 - p2)*(3.0*tt);
// }

// // Build an orthonormal frame from tangent using a safe up-vector selection
// void frameFromTangent(in vec3 T_in, out vec3 T, out vec3 N, out vec3 B)
// {
//     T = normalize(T_in);
//     vec3 up = (abs(T.z) < 0.95) ? vec3(0.0,0.0,1.0) : vec3(0.0,1.0,0.0);
//     N = normalize(cross(up, T));
//     B = cross(T, N);
// }

// void main()
// {
//     // (t, phi) from the static grid
//     float t   = a_position.x;
//     float phi = a_position.y;

//     // unpack instance data
//     vec3 p0 = i_data0.xyz; float radius = i_data0.w;
//     vec3 p1 = i_data1.xyz;
//     vec3 p2 = i_data2.xyz;
//     vec3 p3 = i_data3.xyz;

//     vec3 P, Tan;
//     bezier(p0,p1,p2,p3, t, P, Tan);

//     vec3 T,N,B;
//     frameFromTangent(Tan, T, N, B);

//     float c = cos(phi);
//     float s = sin(phi);
//     vec3 ring = c*N + s*B;

//     vec3 worldPos = P + radius * ring;

//     // optional vertex color from instance w (packed uint->float trick)
//     // Or drive color by uniform based on weight, etc.
//     v_color0 = vec4(1.0, 0.0, 0.0, 1.0); // replace if needed

//     gl_Position = mul(u_viewProj, vec4(worldPos, 1.0));
// }