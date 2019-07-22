#version 430
#define TEXTURE_TILED_FACTOR 800.f

struct TerrainData
{
vec4 color;
vec3 normal;
};

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

uniform sampler2D normalmap;
uniform sampler2D blendMap;
uniform sampler2D backgorundTexture;
uniform sampler2D backgorundTextureNormal;
uniform sampler2D redTexture;
uniform sampler2D redTextureNormal;
uniform sampler2D greenTexture;
uniform sampler2D greenTextureNormal;
uniform sampler2D blueTexture;
uniform sampler2D blueTextureNormal;
uniform sampler2D rockTexture;
uniform sampler2D rockNormalTexture;
uniform sampler2D snowTexture;
uniform sampler2D snowTextureNormal;

in vec2 mapCoord_FS;
in vec4 worldPos;
in vec3 tangent;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

vec4 CalculateColor(sampler2D textureId, float factor, vec2 textCoords)
{
    return texture(textureId, textCoords) * factor;
}

TerrainData CalculateTerrainData()
{
    TerrainData result;

    vec4 blendMapColor = texture(blendMap, mapCoord_FS) ;

    float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b) ;
    vec2 tiledCoords = mapCoord_FS * TEXTURE_TILED_FACTOR;

    vec3 normal = normalize(texture(normalmap, mapCoord_FS).xyz);

    float dist = length(perFrame.cameraPosition - worldPos.xyz);
    float largeDetailRange = 325;
    if (dist < largeDetailRange)
    {
        float attenuation = clamp(-dist/largeDetailRange + 1,0.0,1.0);

        vec3 bitangent = normalize(cross(normal, tangent));
        mat3 TBN = mat3(tangent, bitangent, normal);

        vec3 bumpNormal;
        bumpNormal += (2.f * (texture(backgorundTextureNormal, tiledCoords).rgb) - 1.f) * backTextureAmount;
        bumpNormal += (2.f * (texture(redTextureNormal, tiledCoords).rgb) - 1.f) * blendMapColor.r;
        bumpNormal += (2.f * (texture(greenTextureNormal, tiledCoords).rgb) - 1.f) * blendMapColor.g;
        bumpNormal += (2.f * (texture(blueTextureNormal, tiledCoords).rgb) - 1.f) * blendMapColor.b;
        bumpNormal = normalize(bumpNormal);
        bumpNormal.xy *= attenuation;
        result.normal = normalize(TBN * bumpNormal);
    }
    else
    {
        result.normal = normal;
    }

    float normalFactor = dot(normal, vec3(0.f, 4.f, 0.f));
    if (normalFactor > 1 )
        normalFactor = 1;

    vec4 backgorund_texture_colour;
    backgorund_texture_colour = CalculateColor(backgorundTexture, backTextureAmount, tiledCoords);

    vec4 rColor = CalculateColor(redTexture, blendMapColor.r, tiledCoords);
    vec4 gColor = CalculateColor(greenTexture, blendMapColor.g, tiledCoords);
    vec4 bColor = CalculateColor(blueTexture, blendMapColor.b, tiledCoords);

    result.color = backgorund_texture_colour + rColor + gColor + bColor;
    return result;
}

void main()
{
    TerrainData data = CalculateTerrainData();

    WorldPosOut = worldPos;
    DiffuseOut  = vec4(data.color.xyz, 1.0);
    NormalOut   = vec4(data.normal, 1.f);
    SpecularOut = vec4(0);
}
