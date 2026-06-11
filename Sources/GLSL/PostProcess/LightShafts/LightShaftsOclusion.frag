#version 450

layout (std140, align=16, binding=6) uniform SunBuffer
{
    vec4 ndcSunPosition;  // z - turn off, w -samples
    vec4 sunColor; // w is night
    vec4 blurParams; // x- exposure, y - decay, z - density, w - weight
} sunBuffer;

uniform sampler2D gDepth;

in vec2 textureCoords;
out vec4 outSkyColor;

void main()
{
    float depth = texture(gDepth, textureCoords).r;

    if (depth < 1.0)
    {
        outSkyColor = vec4(0.0, 0.0, 0.0, 1.0);
    } 
    else
    {
        float dist = distance(textureCoords, sunBuffer.ndcSunPosition.xy);
        float sunMask = smoothstep(0.05, 0.0, dist);
        outSkyColor = vec4(sunBuffer.sunColor.xyz * sunMask, 1.0);
    }
}