#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 uView;
uniform mat4 uProj;
uniform float uRadius;

in VS_OUT
{
    vec4 position;
    vec3 color;
} gs_in[];

out vec3 fragPos;
out vec3 fColor;
out vec2 bounds;

void main()
{
    // https://stackoverflow.com/questions/54890543/billboard-like-representation-for-spheres-opengl
    vec4 x_dir = vec4( vec3(uView[0][0], uView[1][0], uView[2][0]), 0.0) * uRadius;
    vec4 y_dir = vec4( vec3(uView[0][1], uView[1][1], uView[2][1]), 0.0) * uRadius;

    fColor = gs_in[0].color;

    fragPos = vec3( uView * (gs_in[0].position - x_dir - y_dir) );
    gl_Position = uProj * uView * (gs_in[0].position - x_dir - y_dir);
    bounds = vec2(-1.0, -1.0);
    EmitVertex();

    fragPos = vec3( uView * (gs_in[0].position - x_dir - y_dir) );
    gl_Position = uProj * uView * (gs_in[0].position + x_dir - y_dir);
    bounds = vec2( 1.0, -1.0);
    EmitVertex();

    fragPos = vec3( uView * (gs_in[0].position - x_dir - y_dir) );
    gl_Position = uProj * uView * (gs_in[0].position - x_dir + y_dir);
    bounds = vec2(-1.0,  1.0);
    EmitVertex();

    fragPos = vec3( uView * (gs_in[0].position - x_dir - y_dir) );
    gl_Position = uProj * uView * (gs_in[0].position + x_dir + y_dir);
    bounds = vec2( 1.0,  1.0);
    EmitVertex();

    EndPrimitive();
}
