#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uLightSpace;

out VS_OUT
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
    vec4 fragPosLightSpace;
} vs_out;

void main(void)
{
    vs_out.fragPos              = vec3(uModel * vec4(aPosition, 1.0));
    vs_out.normal               = mat3(transpose(inverse(uModel))) * aNormal;
    vs_out.texCoord             = aTexCoord;
    vs_out.fragPosLightSpace    = uLightSpace * vec4(vs_out.fragPos, 1.0);
    gl_Position                 = uProj * uView * vec4(vs_out.fragPos, 1.0);
}
