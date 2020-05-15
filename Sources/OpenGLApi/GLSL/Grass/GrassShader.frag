#version 440
#define EPSILON 0.001

layout (location = 0) out vec3 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec3 MaterialSpecular;

uniform sampler2D Texture0;
uniform sampler2DShadow ShadowMap;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 clipPlane;
} perApp;

in GS_OUT
{
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    vec2 texCoord;
    vec3 worldPos;
} fs_in;

bool Is(float v)
{
    return v > 0.5f;
}

float CalculateShadowFactor()
{
    float xOffset = 1.0 / fs_in.shadowMapSize;
    float yOffset = 1.0 / fs_in.shadowMapSize;

    float Factor = 0.0;

    float a = 0;
    for (int y = -1 ; y <= 1 ; y++)
     {
        for (int x = -1 ; x <= 1 ; x++)
         {
            vec2 Offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec3 UVC = vec3(fs_in.shadowCoords.xy + Offsets, fs_in.shadowCoords.z + EPSILON);

            if (texture(ShadowMap, UVC) >  0.f)
                Factor += (fs_in.shadowCoords.w * 0.4f);
           a++;
        }
    }

    float value = (0.5 + (Factor / a));
    if( value > 1 )
    {
        value = 1;
    }
    return value;
}

void main()
{
    vec4 diffTexture = vec4(1.f, 1.f, 1.f, 1.f);

    if (Is(perApp.useTextures.x))
    {
        diffTexture  = texture(Texture0, fs_in.texCoord);
        if(diffTexture.a < .5f) discard; // ?
    }

    float shadowFactor = fs_in.useShadows > 0.5f ? CalculateShadowFactor() : 1.0;

    WorldPosOut      = fs_in.worldPos;
    DiffuseOut       = diffTexture * shadowFactor;
    NormalOut        = vec4(.0f, 1.f, .0f, 1.f);
    MaterialSpecular = vec3(.0f);
}