#version 330

in vec3 tFragPos;
in vec3 tNormal;
in vec2 tUV;

out vec4 fragColor;

uniform sampler2D uMapDiffuse;
uniform sampler2D uNormal;

uniform vec3 uViewPos;

uniform bool uParallax;
uniform bool uNormalMap;
uniform bool uReflections;

struct ReflPrope
{
    vec3 pos;
    vec3 extends;

    samplerCube map;
};

const int MAX_PROPES = 5;
uniform ReflPrope uReflPropes[MAX_PROPES];
uniform int uNumPropes;

/* simple linear falloff */
float prope_influence(vec3 fragPos, vec3 probePos, vec3 probeExtent)
{
    vec3 dist = abs(fragPos - probePos) / probeExtent;
    return max(0.0, 1.0 - length(dist));
}

/* compute intersection point to proxy geometry of probe and redirect reflection vector */
vec3 parallax_correct(const in ReflPrope probe, const vec3 reflPrope)
{
    vec3 box_max = probe.pos + probe.extends;
    vec3 box_min = probe.pos - probe.extends;

    vec3 int_0 = (box_max - tFragPos) / reflPrope;
    vec3 int_1 = (box_min - tFragPos) / reflPrope;

    vec3 int_max = max(int_0, int_1);
    float distance = min(min(int_max.x, int_max.y), int_max.z);

    vec3 int_pos = tFragPos + reflPrope * distance;
    vec3 corrected = normalize(int_pos - probe.pos);

    return corrected;
}

void main(void)
{
    /* scale uv coordinates to make waves smaller */
    vec2 uv = tUV * 5.0;

    vec3 normal = normalize(tNormal);
    if(uNormalMap) { normal = normalize( (texture(uNormal, uv).rbg * 2.0 - 1.0) ); }

    vec3 viewDir = normalize(uViewPos - tFragPos);
    vec3 refl = reflect(-viewDir, normal);

    vec3 diff = texture(uMapDiffuse, uv).rgb;
    vec3 refl_intensity = vec3(0.0);

    /* reflections from cube maps */
    if(uReflections)
    {
        float sum_weight = 0.0;
        for(int i = 0; i < MAX_PROPES; i++)
        {
            if(i >= uNumPropes) { break; }

            vec3 reflPrope = refl;
            {
                /* linear blending weight based on distance to probe position */
                float w = prope_influence(tFragPos, uReflPropes[i].pos, uReflPropes[i].extends);

                /* parallax correct sampling direction */
                if(uParallax) { reflPrope = parallax_correct(uReflPropes[i], reflPrope); }

                sum_weight += w;
                refl_intensity += w * texture(uReflPropes[i].map, reflPrope).rgb;
            }
        }

        /* normalize based on total weights */
        refl_intensity /= sum_weight;
    }

    /* custom blending to water color */
    fragColor = vec4(diff * 0.5 + 0.5 * refl_intensity, 0.9);
}
