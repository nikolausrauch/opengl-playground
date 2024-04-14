#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 uModel;

out VS_OUT
{
    vec4 position;
    vec3 color;
} vs_out;

void main(void)
{
    vs_out.position = uModel * vec4(aPosition, 1.0);
    vs_out.color = aColor;
}
