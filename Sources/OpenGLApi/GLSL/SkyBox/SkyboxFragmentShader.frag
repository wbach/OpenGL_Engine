#version 440

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

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

void main(void)
{
    vec4 texture1     = texture(DayCubeMap, TextureCoords);
    vec4 texture2     = texture(NightCubeMap, TextureCoords);
    vec4 final_colour = mix(texture2, texture1, perMeshObject.blendFactor);

    float factor    = (TextureCoords.y - LowerLimit)/(UpperLimit - LowerLimit );
    factor          = clamp(factor, 0.f, 1.f);
    final_colour    = mix(perMeshObject.fogColour, final_colour, factor);

    WorldPosOut      = vec4(TextureCoords, 1.f);
    DiffuseOut       = final_colour;
    NormalOut        = vec4(0.f, 1.f, 0.f, 0.f); // w = 0; dont use fog on skybox
    MaterialSpecular = vec4(0.f);
}