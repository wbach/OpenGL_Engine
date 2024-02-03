#version 440 core
#define TEXTURE_TILED_FACTOR 800.f

struct TerrainData
{
    vec4 color;
    vec4 normal;
};

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
} perFrame;

layout(binding = 0) uniform sampler2DShadow shadowMap;
layout(binding = 2) uniform sampler2D blendMap;
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
layout(binding = 19) uniform sampler2D rockTexture;
layout(binding = 20) uniform sampler2D rockTextureNormal;

in vec2 mapCoord_FS;
in vec4 worldPos;
in vec3 tangent;

in GS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    vec3 passTangent;
} fs_in;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

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
    new_normal = normalize(new_normal);
    return new_normal;
}

float CalculateShadowFactor()
{
    float xOffset = 1.f/fs_in.shadowMapSize;
    float yOffset = 1.f/fs_in.shadowMapSize;

    float factor = 0.0;

    float a = 0;
    for (int y = -1 ; y <= 1 ; y++)
    {
        for (int x = -1 ; x <= 1 ; x++)
        {
            vec2 offsets = vec2(float(x) * xOffset, float(y) * yOffset);
            vec3 uvc = vec3(fs_in.shadowCoords.xy + offsets, fs_in.shadowCoords.z);

            if (texture(shadowMap, uvc) >  0.f)
                factor += (fs_in.shadowCoords.w * 0.4f);
           a++;
        }
    }
    float value = (.5f + (factor / a));
    if( value > 1.f )
        value = 1.f ;

    return value ;
}

bool Is(float f)
{
    return f > .5f;
}

bool NormalMaping()
{
    float dist = length(perFrame.cameraPosition - fs_in.worldPos.xyz);
    return Is(perApp.useTextures.y) && (dist < perApp.viewDistance.y);
}

const vec2 thresholds = vec2(.5f, .1f);

float GetRockBlendFactor()
{
    float value = 1.f - fs_in.normal.y;

    if (value > thresholds.x)
    {
        value = 1.f;
    }
    else if (value > thresholds.y)
    {
        value = (value - thresholds.y) / (thresholds.x - thresholds.y);
    }
    else
    {
        value = 0.f;
    }

    return value;
}

vec4 CalculateBackgroundColor(vec2 tiledCoords, float backTextureAmount)
{
    float blendFactor = GetRockBlendFactor();
    vec4 backgorundTextureColor = texture(backgorundTexture, tiledCoords) * backTextureAmount * (1.f - blendFactor);
    vec4 rockTextureColor       = texture(rockTexture, tiledCoords) * backTextureAmount * blendFactor;
    return backgorundTextureColor + rockTextureColor;
}

vec4 CalculateTerrainColor(vec2 tiledCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!Is(perApp.useTextures.x))
    {
        return vec4(.8f, .8f, .8f, 1.f);
    }

    vec4 backgorundTextureColour = CalculateBackgroundColor(tiledCoords, backTextureAmount);
    vec4 redTextureColor        = texture(redTexture, tiledCoords) * blendMapColor.r;
    vec4 greenTextureColor      = texture(greenTexture, tiledCoords) * blendMapColor.g;
    vec4 blueTextureColor       = texture(blueTexture, tiledCoords) * blendMapColor.b;
    vec4 alphaTextureColor      = texture(alphaTexture, tiledCoords) * blendMapColor.a;
    return backgorundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 CalculateTerrainNormal(vec2 tiledCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (NormalMaping())
    {
        vec4 backgorundNormalColor   = texture(backgorundTextureNormal, tiledCoords) * backTextureAmount;
        vec4 redNormalTextureColor   = texture(redTextureNormal, tiledCoords) * blendMapColor.r;
        vec4 greenNormalTextureColor = texture(greenTextureNormal, tiledCoords) * blendMapColor.g;
        vec4 blueNormalTextureColor  = texture(blueTextureNormal, tiledCoords) * blendMapColor.b;
        vec4 alphaNormalTextureColor = texture(alphaTextureNormal, tiledCoords) * blendMapColor.a;
        return vec4(CalcBumpedNormal(fs_in.normal, fs_in.passTangent, backgorundNormalColor + redNormalTextureColor + greenNormalTextureColor + blueNormalTextureColor + alphaNormalTextureColor), 1.f); // w use fog
    }
    else
    {
       return vec4(normalize(fs_in.normal), 1.f);
    }
}

TerrainData GetTerrainData()
{
    vec2 tiledCoords   = fs_in.texCoord * 480.0f;
    vec4 blendMapColor = texture(blendMap, fs_in.texCoord);
    float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);

    TerrainData result;
    result.color  = CalculateTerrainColor(tiledCoords, blendMapColor, backTextureAmount);
    result.normal = CalculateTerrainNormal(tiledCoords, blendMapColor, backTextureAmount);
    return result;
}

void main()
{
    TerrainData terrainData = GetTerrainData();
    float shadowFactor = Is(fs_in.useShadows) ? CalculateShadowFactor() : 1.f;

    WorldPosOut     = fs_in.worldPos;
    DiffuseOut      = terrainData.color * shadowFactor;
    NormalOut       = terrainData.normal;
    SpecularOut     = vec4(0.f, 0.f, 0.f, 0.f);
}
