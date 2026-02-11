#version 450

layout (std140, align=16, binding=6) uniform SunBuffer
{
    vec4 ndcSunPosition;  // z - turn off, w -samples
    vec4 sunColor; // w is night
    vec4 blurParams; // x- exposure, y - decay, z - density, w - weight
} sunBuffer;

out vec4 FragColor;
in vec2 textureCoords;

uniform sampler2D occlusionMap; 

float interleaved_gradient_noise(vec2 uv)
{
    return fract(52.11 * fract(dot(uv, vec2(0.0605, 0.0598))));
}

void main()
{
    if (sunBuffer.ndcSunPosition.z < 0.0)
    {
        FragColor = vec4(0.0);
        return;
    }

    const int numSamples = int(sunBuffer.ndcSunPosition.w);
    const float exposure = sunBuffer.blurParams.x;
    const float decay = sunBuffer.blurParams.y;
    const float density = sunBuffer.blurParams.z;
    const float weight = sunBuffer.blurParams.w;

    // Obliczamy krok (wektor przesunięcia)
    vec2 sunUV = sunBuffer.ndcSunPosition.xy;
    vec2 deltaTexCoord = (textureCoords - sunUV);
    deltaTexCoord *= 1.0 / float(numSamples) * density;
    
    float offset = interleaved_gradient_noise(gl_FragCoord.xy);
    vec2 coord = textureCoords + deltaTexCoord * offset;

    float illuminationDecay = 1.0;
    
    vec3 color = texture(occlusionMap, coord).rgb;

    // Pętla tworząca smugi
    for (int i = 0; i < numSamples; i++)
    {
        coord -= deltaTexCoord;
        vec3 sampled = texture(occlusionMap, coord).rgb;
        
        sampled *= illuminationDecay * weight;
        color += sampled;
        
        illuminationDecay *= decay;
    }

    FragColor = vec4(sunBuffer.sunColor.xyz * color * exposure, 1.0);

}