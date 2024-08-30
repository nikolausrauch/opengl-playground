#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 tPosition;

void main(void)
{
    tPosition = aPosition;
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1.0);
}
