#version 330

in vec4 tClipPosCurr;
in vec4 tClipPosPrev;
in vec3 tNormal;
in vec2 tUV;

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec3 gVelocity;

uniform sampler2D uMapDiffuse;

void main(void)
{
    vec4 clip_curr = tClipPosCurr / tClipPosCurr.w;
    clip_curr.xy = clip_curr.xy * 0.5 + 0.5;

    vec4 clip_prev = tClipPosPrev / tClipPosPrev.w;
    clip_prev.xy = clip_prev.xy * 0.5 + 0.5;

    gVelocity = clip_prev.xyz - clip_curr.xyz;
    gColor = texture(uMapDiffuse, tUV);
}