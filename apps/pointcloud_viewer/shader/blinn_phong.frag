#version 430 core

struct Light
{
    vec3 direction;
    vec3 ambient;
    vec3 color;
};

struct Material
{
    float shininess;
    vec3 specular;
};

uniform Light uLight;
uniform Material uMaterial;

in vec3 fragPos;
in vec3 fColor;
in vec2 bounds;

out vec4 fragColor;


vec3 brdf_blinn_phong(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 diffuse, vec3 specular, float shininess)
{
    vec3 halfwayDir = normalize(lightDir + viewDir);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    return (diff * diffuse) + (spec * specular);
}


void main(void)
{
    if(dot(bounds, bounds) > 1.0)
    {
        discard;
    }

    vec3 normal = normalize(vec3(bounds, 1.0 - length(bounds)));
    vec3 lightDir = normalize(-uLight.direction);
    vec3 viewDir = normalize(-fragPos);

    vec3 illuminance = uLight.ambient * fColor;
    illuminance += uLight.color * brdf_blinn_phong(lightDir, viewDir, normal,
                                                   fColor,
                                                   uMaterial.specular,
                                                   uMaterial.shininess);

    fragColor = vec4(illuminance, 1.0);
}
