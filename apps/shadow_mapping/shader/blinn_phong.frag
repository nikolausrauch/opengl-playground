#version 330

struct Light
{
    vec3 direction;

    vec3 ambient;
    vec3 color;

    float bias;
    float spacing;
    int samples;
};

struct Shadow
{
    float bias;
    float spacing;
    int samples;

    sampler2D map_shadow;
};

struct Material
{
    float shininess;

    sampler2D map_diffuse;
    sampler2D map_specular;
};


uniform vec3 uViewPos;
uniform Light uLight;
uniform Shadow uShadow;
uniform Material uMaterial;

out vec4 fragColor;

in VS_OUT
{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoord;
    vec4 fragPosLightSpace;
} fs_in;


// https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/ShadowMapping.fragmentshader
float random(vec3 seed, int i)
{
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float compute_light_coverage(vec4 fragPosLightSpace, vec3 lightDirection, float bias, float spacing, int samples)
{
    // https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/ShadowMapping.fragmentshader
    vec2 poission_disc[16] =
        vec2[](
            vec2(-0.94201624,  -0.39906216 ),
            vec2( 0.94558609,  -0.76890725 ),
            vec2(-0.094184101, -0.92938870 ),
            vec2( 0.34495938,   0.29387760 ),
            vec2(-0.91588581,   0.45771432 ),
            vec2(-0.81544232,  -0.87912464 ),
            vec2(-0.38277543,   0.27676845 ),
            vec2( 0.97484398,   0.75648379 ),
            vec2( 0.44323325,  -0.97511554 ),
            vec2( 0.53742981,  -0.47373420 ),
            vec2(-0.26496911,  -0.41893023 ),
            vec2( 0.79197514,   0.19090188 ),
            vec2(-0.24188840,   0.99706507 ),
            vec2(-0.81409955,   0.91437590 ),
            vec2( 0.19984126,   0.78641367 ),
            vec2( 0.14383161,  -0.14100790 )
        );

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float adjBias = max(bias * (1.0 - dot(fs_in.normal, lightDirection)), bias / 10.0);
    //float adjBias = clamp( bias * tan(acos( dot(fs_in.normal, lightDirection) ) ), 0, 2 * bias);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadow.map_shadow, 0);

    for(int i = 0; i < samples; i++)
    {
        int index = int( 16.0 * random(gl_FragCoord.xyy, i) ) % 16;
        float closestDepth = texture( uShadow.map_shadow, projCoords.xy + poission_disc[index] * texelSize * spacing ).r;
        shadow += currentDepth - adjBias > closestDepth ? 1.0 : 0.0;
    }

    shadow = projCoords.z > 1.0 ? 0.0 : shadow / samples;

    return shadow;
}

vec3 brdf_blinn_phong(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 diffuse, vec3 specular, float shininess)
{
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    return (diff * diffuse) + (spec * specular);
}

void main(void)
{
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = normalize(-uLight.direction);
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);

    vec3 illuminance = uLight.ambient * vec3(texture(uMaterial.map_diffuse, fs_in.texCoord));

    float shadow = compute_light_coverage(fs_in.fragPosLightSpace, lightDir, uShadow.bias, uShadow.spacing, uShadow.samples);
    illuminance += (1.0 - shadow) * uLight.color * brdf_blinn_phong(lightDir, viewDir, normal,
                                                   vec3(texture(uMaterial.map_diffuse, fs_in.texCoord)),
                                                   vec3(texture(uMaterial.map_specular, fs_in.texCoord)),
                                                   uMaterial.shininess);

    fragColor = vec4(illuminance, 1.0);
}
