#version 330

struct Light
{
    vec3 color;
    vec3 ambient;
    vec3 direction;
};

struct Material
{
    float shininess;
    vec3 specular;

    sampler2D map_diffuse;
};

in VS_OUT
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
} fs_in;

uniform vec3 uViewPos;
uniform Light uLight;
uniform Material uMaterial;

uniform sampler2D intensity_map;

layout (location = 0) out vec4 oFragColor;

void main(void)
{
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(-uLight.direction);
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);

    /* Blinn-Phong */
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMaterial.shininess);

    /* map to intensity buckets */
    diff = texture(intensity_map, vec2(diff, 0.5)).r;
    spec = texture(intensity_map, vec2(spec, 0.5)).r;

    vec3 mat_diffuse = texture(uMaterial.map_diffuse, fs_in.texCoord).rgb;

    vec3 illuminance = uLight.ambient * mat_diffuse;
    illuminance += (diff * mat_diffuse) + (spec * uMaterial.specular);

    oFragColor = vec4(uLight.color * illuminance, 1.0);
}
