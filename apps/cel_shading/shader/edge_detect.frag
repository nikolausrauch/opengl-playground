#version 430

uniform vec2 uNearFar;

uniform sampler2D uColorTexture;
uniform sampler2D uDepthTexture;
uniform float uStepSize;
uniform float uThreshold;

in vec2 tUV;
out vec4 out_color;


/* https://gamedev.stackexchange.com/questions/159585/sobel-edge-detection-on-depth-texture */
mat3 sobel_y = mat3(
     1.0, 0.0, -1.0,
     2.0, 0.0, -2.0,
     1.0, 0.0, -1.0
);

mat3 sobel_x = mat3(
     1.0, 2.0, 1.0,
     0.0, 0.0, 0.0,
    -1.0, -2.0, -1.0
);

float linearizeDepth(float depth)
{
    float z_n = 2.0 * depth - 1.0;
    float linearDepth = (2.0 * uNearFar.x * uNearFar.y) / (uNearFar.y + uNearFar.x - z_n * (uNearFar.y - uNearFar.x));
    return clamp(linearDepth / uNearFar.y, 0.0, 1.0);
}

void main()
{
    vec4 color = texture(uColorTexture, tUV);
    vec2 texSize  = textureSize(uDepthTexture, 0).xy;

    /* https://gamedev.stackexchange.com/questions/159585/sobel-edge-detection-on-depth-texture */
    mat3 I = mat3(0.0);
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            vec2 step = uStepSize * vec2(i-1, j-1) / texSize;
            float depth = linearizeDepth(texture(uDepthTexture, tUV + step).r);
            I[i][j] = depth;
        }
    }

    float gx = dot(sobel_x[0], I[0]) + dot(sobel_x[1], I[1]) + dot(sobel_x[2], I[2]);
    float gy = dot(sobel_y[0], I[0]) + dot(sobel_y[1], I[1]) + dot(sobel_y[2], I[2]);

    float g = sqrt(pow(gx, 2.0) + pow(gy, 2.0));
    float color_mask = uThreshold < g ? 0.0 : 1.0;

    out_color = vec4( vec3(color) * color_mask, 1.0 );
}
