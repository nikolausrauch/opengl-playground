#version 330

struct Material
{
    float shininess;
    sampler2D map_diffuse;
};

uniform Material uMaterial;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gMaterial;

in VS_OUT
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
} fs_in;

void main(void)
{
    gPosition = fs_in.fragPos;
    gNormal = fs_in.normal;
    gMaterial = vec4(texture(uMaterial.map_diffuse, fs_in.texCoord).rgb, uMaterial.shininess / 255.0);
}
