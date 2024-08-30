#version 330

out vec4 fragColor;

uniform sampler2D uEntryTex;
uniform sampler2D uExitTex;
uniform sampler3D uVolumeTex;

uniform vec2 uScreenSize;

uniform float uStepSize;
uniform int uMaxStep;

uniform float uGamma = 1.5;

void main(void)
{
    vec2 fragPos = gl_FragCoord.xy / uScreenSize;
    vec3 rayStart = texture(uEntryTex, fragPos).xyz;
    vec3 rayEnd = texture(uExitTex, fragPos).xyz;
    vec3 rayDir = normalize(rayEnd - rayStart);

    uint numSteps = uint( min(uMaxStep, length(rayStart - rayEnd) / uStepSize) );

    vec3 rayPos = rayStart;
    vec4 finalColor = vec4(0.0);
    for(uint i = 0u; i < numSteps; i++)
    {
        float density = textureLod(uVolumeTex, rayPos, 0).r;
        finalColor = max(finalColor, vec4(density));
        rayPos += uStepSize * rayDir;
    }

    fragColor.a = 1.0;
    fragColor.rgb = pow(finalColor.rgb, vec3(1.0 / uGamma));
}
