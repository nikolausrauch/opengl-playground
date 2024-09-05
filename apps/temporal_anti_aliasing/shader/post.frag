#version 330

uniform sampler2D uColorPrev;
uniform sampler2D uColorCurr;
uniform sampler2D uVelocity;
uniform float uAlpha;

out vec4 fragColor;

void main(void)
{
    vec2 texCoord = gl_FragCoord.xy / vec2(textureSize(uColorPrev, 0));
    vec2 velocity = texture(uVelocity, texCoord).xy;

    vec4 color = texture(uColorCurr, texCoord);
    vec4 colorPrev = texture(uColorPrev, texCoord + velocity);

    /* History rectification: AABB clamping */
    vec4 color_0 = textureOffset(uColorCurr, texCoord, ivec2( 1,  0));
    vec4 color_1 = textureOffset(uColorCurr, texCoord, ivec2( 0,  1));
    vec4 color_2 = textureOffset(uColorCurr, texCoord, ivec2(-1,  0));
    vec4 color_3 = textureOffset(uColorCurr, texCoord, ivec2( 0, -1));

    vec4 boxMin = min(color, min(color_0, min(color_1, min(color_2, color_3))));
    vec4 boxMax = max(color, max(color_0, max(color_1, max(color_2, color_3))));;

    colorPrev = clamp(colorPrev, boxMin, boxMax);

    fragColor = mix(colorPrev, color, uAlpha);
}
