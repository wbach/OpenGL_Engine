#version 440
#define EPSILON 0.0002

layout (std140, align=16, binding=6) uniform PerMeshObject
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    uint numberOfRows;
    float useTexture;
    float useNormalMap;
    float useSpecularMap;
    float shineDamper;
    float useFakeLighting;
} perMeshObject;

in VS_OUT
{
    vec2 textureOffset;
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    mat3 tbn;
} fs_in;

layout(binding = 4) uniform sampler2DShadow ShadowMap;
layout(binding = 0) uniform sampler2D DiffuseTexture;
layout(binding = 1) uniform sampler2D AmbientTexture;
layout(binding = 2) uniform sampler2D NormalMap;
layout(binding = 3) uniform sampler2D SpecularMap;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

vec4 CalcBumpedNormal(vec2 text_coords)
{
    vec3 bumpMapNormal = texture(NormalMap, text_coords).xyz;
    bumpMapNormal = bumpMapNormal * 2.f - 1.f;
    return vec4(normalize(fs_in.tbn * bumpMapNormal) , 1.f);
}

bool Is(float v)
{
    return v > 0.5f;
}

vec4 GetNormal(vec2 textCoord)
{
    if (Is(perMeshObject.useFakeLighting))
    {
        return vec4(0.0f, 1.0f, 0.0f, 1.f); // w use fog
    }
    else
    {
        return Is(perMeshObject.useNormalMap) ? CalcBumpedNormal(textCoord) : vec4(normalize(fs_in.normal), 1.f);
    }
}

vec4 GetSpecular(vec2 textCoord)
{
    if (Is(perMeshObject.useSpecularMap))
    {
        return vec4((texture(SpecularMap, textCoord) * perMeshObject.specular).xyz , perMeshObject.shineDamper);

    }
    else
    {
       return vec4(perMeshObject.specular.xyz, perMeshObject.shineDamper);
    }
}

void main()
{
    vec4 colorFromTexture = vec4(1.f, 1.f, 1.f, 1.f);
    vec2 textCoord = (fs_in.texCoord / perMeshObject.numberOfRows) + fs_in.textureOffset;

    if (Is(perMeshObject.useTexture))
    {
        colorFromTexture = texture(DiffuseTexture, textCoord);
        if(!Is(colorFromTexture.a))
        {
            discard;
        }
    }

    WorldPosOut      = fs_in.worldPos;
    NormalOut        = GetNormal(textCoord);
    DiffuseOut       = colorFromTexture * perMeshObject.diffuse;
    MaterialSpecular = GetSpecular(textCoord);
}
