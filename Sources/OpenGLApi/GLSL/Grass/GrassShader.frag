#version 440
#define EPSILON 0.001

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

uniform sampler2D Texture0;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

in VS_OUT
{
    vec2 texCoord;
    vec4 worldPos;
    vec3 normal;
    vec3 color;
} fs_in;

bool Is(float v)
{
    return v > 0.5f;
}

void main()
{
    vec4 diffTexture = vec4(1.f, 1.f, 1.f, 1.f);

    if (Is(perApp.useTextures.x))
    {
        diffTexture  = texture(Texture0, fs_in.texCoord);
        if(diffTexture.a < .5f) discard;
    }

    vec3 normal = fs_in.normal;
    WorldPosOut      = fs_in.worldPos;
    DiffuseOut       = diffTexture * vec4(fs_in.color, 1.f);
    NormalOut        = vec4(normal, 1.f);
    MaterialSpecular = vec4(.0f);
}