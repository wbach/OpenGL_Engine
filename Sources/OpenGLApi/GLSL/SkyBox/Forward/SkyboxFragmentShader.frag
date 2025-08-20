#version 440 core

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    float blendFactor;
} perMeshObject;

in vec3 TextureCoords;

layout(binding = 0) uniform samplerCube DayCubeMap;
layout(binding = 1) uniform samplerCube NightCubeMap;

const float LowerLimit = 0.0;
const float UpperLimit = 0.1;

out vec4 outputColor;

void main(void)
{
    vec4 texture1     = texture(DayCubeMap, TextureCoords);
    vec4 texture2     = texture(NightCubeMap, TextureCoords);
    vec4 finalColor = mix(texture2, texture1, perMeshObject.blendFactor);

    float factor  = (TextureCoords.y - LowerLimit)/(UpperLimit - LowerLimit );
    factor        = clamp(factor, 0.f, 1.f);
    outputColor   = mix(vec4(perApp.fogData.xyz, 1.f), finalColor, factor);
    outputColor.w = 1.f;
}