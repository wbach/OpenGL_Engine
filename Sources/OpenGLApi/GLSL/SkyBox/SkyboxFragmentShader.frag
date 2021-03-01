#version 440

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

uniform samplerCube DayCubeMap;
uniform samplerCube NightCubeMap;

const float LowerLimit = 0.0;
const float UpperLimit = 0.1;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main(void)
{
    vec4 texture1   = texture(DayCubeMap, TextureCoords);
    vec4 texture2   = texture(NightCubeMap, TextureCoords);
    vec4 finalColor = mix(texture2, texture1, perMeshObject.blendFactor);

    float fogFactor = (TextureCoords.y - LowerLimit)/(UpperLimit - LowerLimit );

    WorldPosOut      = vec4(TextureCoords, 1.f);
    DiffuseOut       = mix(vec4(perApp.fogData.xyz, 1.f), finalColor, clamp(fogFactor, 0.f, 1.f));
    NormalOut        = vec4(0.f, 1.f, 0.f, 0.f); // w = 0; dont use fog on skybox,
    MaterialSpecular = vec4(0.f);
}