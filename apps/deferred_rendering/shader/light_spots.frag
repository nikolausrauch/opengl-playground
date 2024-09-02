#version 430

struct Light
{
    vec3 position;
    vec3 direction;

    vec3 color;

    float innerCutoff;
    float outerCutoff;

    float constant;
    float linear;
    float quadratic;
};

layout(std430, binding = 0) buffer bLights
{
    Light lights[];
};

struct Buffer
{
    sampler2D pos;
    sampler2D normal;
    sampler2D material;
};

uniform vec3 uViewPos;
uniform vec2 uViewSize;
uniform int uLightIndex;
uniform Buffer gBuffer;


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
    Light uLight = lights[uLightIndex];
    vec2 texCoord = gl_FragCoord.xy / uViewSize.xy;
    vec3 fragPos = texture(gBuffer.pos, texCoord).rgb;
    vec3 normal = normalize(texture(gBuffer.normal, texCoord).rgb);

    vec4 material = texture(gBuffer.material, texCoord).rgba;
    vec3 diffuse = material.rgb;
    float shininess = material.a * 256.0;
    vec3 specular = diffuse;

    vec3 lightDir = normalize(uLight.position - fragPos);
    vec3 spotDir = normalize(-uLight.direction);
    vec3 viewDir = normalize(uViewPos - fragPos);

    float theta = acos(dot(lightDir, spotDir));
    float epsilon = (uLight.outerCutoff - uLight.innerCutoff);
    float intensity = clamp((uLight.outerCutoff - theta) / epsilon, 0.0, 1.0);

    float distance    = length(uLight.position - fragPos);
    float attenuation = 1.0 / (uLight.constant + uLight.linear * distance + uLight.quadratic * (distance * distance));

    vec3 illuminance = intensity * attenuation * uLight.color * brdf_blinn_phong(lightDir, viewDir, normal, diffuse, specular, shininess);

    fragColor = vec4(illuminance, 1.0);
}
