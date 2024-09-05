#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uProjViewCurr;
uniform mat4 uProjViewPrev;

uniform vec2 uJitter;

out vec4 tClipPosCurr;
out vec4 tClipPosPrev;
out vec3 tNormal;
out vec2 tUV;

void main(void)
{
    vec4 worldPos = uModel * vec4(aPosition, 1.0);

    tClipPosPrev = uProjViewPrev * worldPos;
    tClipPosCurr = uProjViewCurr * worldPos;

    gl_Position = tClipPosCurr;
    gl_Position.xy += 2.0 * uJitter * gl_Position.w;

    tNormal = aNormal;
    tUV = aUV;
}
    
