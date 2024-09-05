#version 330

in vec3 tNormal;
in vec2 tUV;

out vec4 fragColor;

uniform sampler2D uMapDiffuse;

void main(void)
{
    fragColor = texture(uMapDiffuse, tUV).rgba;
}
