#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;

out vec3 tNormal;
out vec2 tUVs;

void main(void)
{
    gl_Position = uModel * vec4(aPosition, 1.0);
    tNormal = aNormal;
    tUVs = aUV;
}
