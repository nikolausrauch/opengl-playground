#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec2 tUVs[];

uniform mat4 captureViews[6];
uniform mat4 projection;

out vec2 tUV;
out vec3 tFragPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        mat4 VP = projection * captureViews[face];

        for (int i = 0; i < 3; ++i)
        {
            tFragPos = gl_in[i].gl_Position.xyz;
            tUV = tUVs[i];
            gl_Position = VP * gl_in[i].gl_Position;

            EmitVertex();
        }
        EndPrimitive();
    }
}
