#version 330

struct Light
{
    vec3 direction;

    vec3 ambient;
    vec3 color;
};

struct Buffer
{
    sampler2D pos;
    sampler2D normal;
    sampler2D material;
};

uniform vec3 uViewPos;
uniform vec2 uViewSize;
uniform Light uLight;
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
    vec2 texCoord = gl_FragCoord.xy / uViewSize.xy;
    vec3 fragPos = texture(gBuffer.pos, texCoord).rgb;
    vec3 normal = normalize(texture(gBuffer.normal, texCoord).rgb);
    vec4 material = texture(gBuffer.material, texCoord).rgba;

    vec3 diffuse = material.rgb;
    vec3 specular = diffuse;
    float shininess = material.a * 256.0f;

    vec3 lightDir = normalize(-uLight.direction);
    vec3 viewDir = normalize(uViewPos - fragPos);

    vec3 illuminance = uLight.ambient * diffuse;

    illuminance += uLight.color * brdf_blinn_phong(lightDir, viewDir, normal,
                                                   diffuse,
                                                   specular,
                                                   shininess);

    fragColor = vec4(illuminance, 1.0);
}
