#version 440

layout(binding = 0) uniform sampler2D HDRBuffer; // HDR input (Rgba32f)

layout (std140, align=16, binding=3) uniform ToneMappingBuffer
{
    vec4 param;
} toneMappingBuffer;

in vec2 textureCoords;
out vec4 FragColor;

// ACES-like tonemapping
vec3 ACESFilmicTonemap(vec3 color)
{
    // standardowy parametr ACES
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((color*(a*color + b)) / (color*(c*color + d) + e), 0.0, 1.0);
}

vec3 ApplyGamma(vec3 color, float gamma)
{
    return pow(color, vec3(1.0 / gamma));
}

void main()
{
    // Exposure i gamma
    const float exposure = toneMappingBuffer.param.y;
    const float gamma = toneMappingBuffer.param.x;

    // 1. Pobranie HDR koloru
    vec3 hdrColor = texture(HDRBuffer, textureCoords).rgb;

   // 2. Ekspozycja
    hdrColor *= exposure;

    // 3. Tonemapping ACES
    vec3 ldrColor = ACESFilmicTonemap(hdrColor);

    // 4. Gamma correction
    ldrColor = ApplyGamma(ldrColor, gamma);

    FragColor = vec4(ldrColor, 1.0);
}
