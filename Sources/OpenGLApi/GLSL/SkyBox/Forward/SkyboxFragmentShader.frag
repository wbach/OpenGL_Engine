#version 440 core

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 fogColour;
    float blendFactor;
} perMeshObject;

in vec3 TextureCoords;

uniform samplerCube DayCubeMap;
uniform samplerCube NightCubeMap;

const float LowerLimit = 0.0;
const float UpperLimit = 0.2;

out vec4 outputColor;

void main(void)
{
    vec4 texture1     = texture(DayCubeMap, TextureCoords);
    vec4 texture2     = texture(NightCubeMap, TextureCoords);
    vec4 final_colour = mix(texture2, texture1, perMeshObject.blendFactor);

    float factor    = (TextureCoords.y - LowerLimit)/(UpperLimit - LowerLimit );
    factor          = clamp(factor, 0.f, 1.f);
    outputColor    = mix(perMeshObject.fogColour, final_colour, factor);
}