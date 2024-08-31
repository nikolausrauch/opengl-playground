#version 330

out vec4 fragColor;

struct Raycast
{
    sampler2D entry;
    sampler2D exit;
    sampler3D volume;

    float stepSize;
    int maxSteps;

    int renderType;
    float isoValue;
    float gamma;
};

struct Light
{
    vec3 color;
    vec3 ambient;
    vec3 direction;
};

uniform mat4 uModel;
uniform vec2 uScreenSize;

uniform Light uLight;
uniform Raycast uRaycast;


// https://github.com/opengl-tutorials/ogl/blob/master/tutorial16_shadowmaps/ShadowMapping.fragmentshader
float random(vec3 seed, int i)
{
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

vec3 refine(in vec3 pos, in vec3 rayDir)
{
    /* half step */
    vec3 step_vector = rayDir*uRaycast.stepSize;
    pos -= (0.5 * step_vector);

    /* quarter step */
    float density = textureLod(uRaycast.volume, pos, 0).r;
    pos -= step_vector * (density > uRaycast.isoValue ? 0.25 : -0.25 );

    return pos;
}

vec3 gradient(vec3 position)
{
    /* central difference approximation of density gradient */
    float dx = textureLodOffset(uRaycast.volume, position, 0, ivec3( 1, 0, 0)).r
              -textureLodOffset(uRaycast.volume, position, 0, ivec3(-1, 0, 0)).r;
    float dy = textureLodOffset(uRaycast.volume, position, 0, ivec3( 0, 1, 0)).r
              -textureLodOffset(uRaycast.volume, position, 0, ivec3( 0,-1, 0)).r;
    float dz = textureLodOffset(uRaycast.volume, position, 0, ivec3( 0, 0, 1)).r
              -textureLodOffset(uRaycast.volume, position, 0, ivec3( 0, 0,-1)).r;

    return vec3(dx, dy, dz) * 0.5;
}

vec4 render_mip(in vec3 rayPos, in vec3 rayDir, in uint numSteps)
{
    vec4 finalColor = vec4(0.0);
    for(uint i = 0u; i < numSteps; i++)
    {
        float density = textureLod(uRaycast.volume, rayPos, 0).r;
        finalColor = max(finalColor, vec4(density));
        rayPos += uRaycast.stepSize * rayDir;
    }

    return vec4(pow(finalColor.rgb, vec3(1.0 / uRaycast.gamma)), 1.0);
}

vec4 render_isosurface(in vec3 rayPos, in vec3 rayDir, in uint numSteps)
{
    vec4 finalColor = vec4(0.0);
    for(uint i = 0u; i < numSteps; i++)
    {
        float density = textureLod(uRaycast.volume, rayPos, 0).r;
        if(density > uRaycast.isoValue)
        {
            /* refine position and compute normal estimate from gradient */
            rayPos = refine(rayPos, rayDir);
            vec3 normal = mat3(uModel) * -normalize(gradient(rayPos));

            /* shading */
            vec3 lightDir = normalize(-uLight.direction);
            float diff = max(dot(normal, lightDir), 0.0);
            finalColor.rgb = uLight.ambient + uLight.color * diff;

            break;
        }

        rayPos += uRaycast.stepSize * rayDir;
    }

    finalColor.a = 1.0;
    return finalColor;
}

void main(void)
{
    vec2 fragPos = gl_FragCoord.xy / uScreenSize;
    vec3 rayStart = texture(uRaycast.entry, fragPos).xyz;
    vec3 rayEnd = texture(uRaycast.exit, fragPos).xyz;
    vec3 rayDir = normalize(rayEnd - rayStart);

    if(rayStart == rayEnd) { discard; }

    uint numSteps = uint( min(uRaycast.maxSteps, length(rayStart - rayEnd) / uRaycast.stepSize) );
    float random_offset = random(gl_FragCoord.xyy, int(gl_FragCoord.x));
    vec3 rayPos = rayStart + rayDir * random_offset * uRaycast.stepSize;

    switch (uRaycast.renderType)
    {
    case 0:
        fragColor = render_isosurface(rayStart, rayDir, numSteps);
        break;
    case 1:
        fragColor = render_mip(rayStart, rayDir, numSteps);
        break;
    }
}
