#version 440

struct TerrainNormalColor
{
    vec4 color;
    vec4 normal;
};

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    vec3 passTangent;
    float useNormalMap;
} vs_in;

layout(binding = 0) uniform sampler2DShadow shadowMap;
layout(binding = 2) uniform sampler2D blendMap;
layout(binding = 3) uniform sampler2D normalmap;
layout(binding = 4) uniform sampler2D backgorundTexture;
layout(binding = 5) uniform sampler2D backgorundTextureNormal;
layout(binding = 7) uniform sampler2D redTexture;
layout(binding = 8) uniform sampler2D redTextureNormal;
layout(binding = 10) uniform sampler2D greenTexture;
layout(binding = 11) uniform sampler2D greenTextureNormal;
layout(binding = 13) uniform sampler2D blueTexture;
layout(binding = 14) uniform sampler2D blueTextureNormal;
layout(binding = 16) uniform sampler2D alphaTexture;
layout(binding = 17) uniform sampler2D alphaTextureNormal;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

bool Is(float f)
{
    return f > .5f;
}

vec3 CalcBumpedNormal(vec3 surface_normal, vec3 pass_tangent, vec4 normal_map)
{
    vec3 normal  = normalize(surface_normal);
    vec3 tangent = normalize(pass_tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);
    vec3 bumpMapNormal = normal_map.xyz ;
    bumpMapNormal = 2.f * bumpMapNormal - vec3(1.f, 1.f, 1.f);
    vec3 new_normal;
    mat3 tbn = mat3(tangent, bitangent, normal);
    new_normal = tbn * bumpMapNormal;
   // new_normal.y *= 1.25f;
    new_normal = normalize(new_normal);
    return new_normal;
}

float CalculateShadowFactor()
{
    float xOffset = 1.0/vs_in.shadowMapSize;
    float yOffset = 1.0/vs_in.shadowMapSize;

    float factor = 0.0;

    float a = 0;
    for (int y = -1 ; y <= 1 ; y++)
    {
        for (int x = -1 ; x <= 1 ; x++)
        {
            vec2 offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec3 uvc = vec3(vs_in.shadowCoords.xy + offsets, vs_in.shadowCoords.z);

            if (texture(shadowMap, uvc) >  0.f)
                factor += (vs_in.shadowCoords.w * 0.4f);
           a++;
        }
    }
    float value = (.5f + (factor / a));
    if( value > 1.f )
        value = 1.f ;

    return value ;
}

TerrainNormalColor CalculateTerrainColor()
{
    TerrainNormalColor result;
    vec4 blendMapColour = texture(blendMap, vs_in.texCoord);

    float backTextureAmount = 1.f - (blendMapColour.r + blendMapColour.g + blendMapColour.b + blendMapColour.a);
    vec2 tiledCoords        = vs_in.texCoord * 40.0f ;

    vec4 backgorundTextureColour = texture(backgorundTexture, tiledCoords) * backTextureAmount;

    vec4 redTextureColour   = texture(redTexture, tiledCoords) * blendMapColour.r;
    vec4 greenTextureColour = texture(greenTexture, tiledCoords) * blendMapColour.g;
    vec4 blueTextureColour  = texture(blueTexture, tiledCoords) * blendMapColour.b;
    vec4 alphaTextureColour = texture(alphaTexture, tiledCoords) * blendMapColour.a;

    if (Is(vs_in.useNormalMap))
    {
        vec4 backgorund_n_texture_colour = texture(backgorundTextureNormal, tiledCoords) * backTextureAmount;
        vec4 redNormalTextureColour      = texture(redTextureNormal, tiledCoords) * blendMapColour.r;
        vec4 greenNormalTextureColour    = texture(greenTextureNormal, tiledCoords) * blendMapColour.g;
        vec4 blueNormalTextureColour     = texture(blueTextureNormal, tiledCoords) * blendMapColour.b;
        vec4 alphaNormalTextureColour    = texture(alphaTextureNormal, tiledCoords) * blendMapColour.a;
        result.normal = vec4(CalcBumpedNormal(vs_in.normal, vs_in.passTangent, backgorund_n_texture_colour + redNormalTextureColour + greenNormalTextureColour + blueNormalTextureColour + alphaNormalTextureColour), 1.f); // w use fog
    }
    else
    {
       result.normal = vec4(normalize(vs_in.normal), 1.f);
    }

    result.color = backgorundTextureColour + redTextureColour + greenTextureColour + blueTextureColour + alphaTextureColour;
    return result;
}

void main()
{
    TerrainNormalColor terrainNormalColor = CalculateTerrainColor();
    float shadowFactor = Is(vs_in.useShadows) ? CalculateShadowFactor() : 1.f;
    WorldPosOut     = vs_in.worldPos;
    DiffuseOut      = terrainNormalColor.color * shadowFactor;
    SpecularOut     = vec4(0.f);
    NormalOut       = terrainNormalColor.normal;
}