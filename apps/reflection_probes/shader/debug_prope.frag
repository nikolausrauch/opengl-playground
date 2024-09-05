#version 330

in vec3 tNormal;
in vec2 tUV;

out vec4 fragColor;

uniform bool uSelected;

void main(void)
{
    const float alpha = 0.2;
    fragColor = uSelected ? vec4(0.0, 1.0, 0.0, alpha) : vec4(1.0, 0.0, 0.0, alpha);
}
