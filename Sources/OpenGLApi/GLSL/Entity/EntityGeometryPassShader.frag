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
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec3 passTangent;
    vec2 textureOffset;
} vs_in;

uniform sampler2DShadow ShadowMap;
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 MaterialSpecular;

vec4 CalcBumpedNormal(vec3 surface_normal, vec3 pass_tangent, sampler2D normal_map, vec2 text_coords)
{
    vec3 normal  = normalize(surface_normal);
    vec3 tangent = normalize(pass_tangent);

    tangent = normalize(tangent - dot(tangent, normal) * normal);

    vec3 bitangent = cross(tangent, normal);
    vec3 bumpMapNormal = texture(normal_map, text_coords).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 new_normal;
    mat3 tbn = mat3(tangent, bitangent, normal);
    new_normal = tbn * bumpMapNormal;
    new_normal = normalize(new_normal);
    return vec4(new_normal, 1.f);
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
        return Is(perMeshObject.useNormalMap) ?  CalcBumpedNormal(vs_in.normal, vs_in.passTangent, NormalMap, textCoord) :  vec4(normalize(vs_in.normal), 1.f);
    }
}

vec4 GetSpecular(vec2 textCoord)
{
    if (Is(perMeshObject.useSpecularMap))
    {
        return vec4((texture(SpecularMap, textCoord) * perMeshObject.specular).xyz, perMeshObject.shineDamper);

    }
    else
    {
       return vec4(perMeshObject.specular.xyz, perMeshObject.shineDamper);
    }
}

void main()
{
    vec4 colorFromTexture = vec4(1.f, 1.f, 1.f, 1.f);
    vec2 textCoord = (vs_in.texCoord / perMeshObject.numberOfRows) + vs_in.textureOffset;

    if (Is(perMeshObject.useTexture))
    {
        colorFromTexture = texture(DiffuseTexture, textCoord);
        if(!Is(colorFromTexture.a))
        {
            discard;
        }
    }

    WorldPosOut      = vs_in.worldPos;
    NormalOut        = GetNormal(textCoord);
    DiffuseOut       = colorFromTexture * perMeshObject.diffuse;
    MaterialSpecular = GetSpecular(textCoord);
}
