#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out VS_OUT
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
} vs_out;

void main(void)
{
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
    vs_out.fragPos = vec3(uModel * vec4(aPosition, 1.0));
    vs_out.normal = mat3(transpose(inverse(uModel))) * aNormal;
    vs_out.texCoord = aUV;
}
