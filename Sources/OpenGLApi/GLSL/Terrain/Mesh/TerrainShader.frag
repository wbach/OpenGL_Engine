#version 440 core
const float EPSILON = 0.00001f;

struct TerrainData
{
    vec4 color;
    vec4 normal;
};

in GS_OUT
{
    vec2 texCoord;
    vec3 normal;
    vec4 worldPos;
    vec4 shadowCoords;
    float useShadows;
    float shadowMapSize;
    mat3 tbn;
    vec3 faceNormal;
} fs_in;

layout(binding = 0) uniform sampler2D shadowMap;
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
layout(binding = 19) uniform sampler2D rockTexture;
layout(binding = 20) uniform sampler2D rockTextureNormal;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 clipPlane;
} perApp;

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding = 6) uniform PerTerrainTexturesBuffer
{
    vec4 rgbaTextureScales;
    vec4 backgroundTextureScales;
} perTerrainTextures;

vec3 CalcBumpedNormal(vec4 normalMapColor)
{
    vec3 bumpMapNormal = normalMapColor.xyz;
    bumpMapNormal = bumpMapNormal * 2.f - 1.f;
    return normalize(fs_in.tbn * bumpMapNormal);
}

float CalculateShadowFactor()
{
    float onl = texture(shadowMap, fs_in.shadowCoords.xy).r;
    float lightFactor = 1.f;

    if (fs_in.shadowCoords.z > onl)
    {
        lightFactor = 1.f -  (fs_in.shadowCoords.w * .4f);
    }
    return lightFactor;
}

// float CalculateShadowFactor()
// {
//     float xOffset = 1.0/fs_in.shadowMapSize;
//     float yOffset = 1.0/fs_in.shadowMapSize;

//     float factor = 0.0;

//     float a = 0;
//     for (int y = -1 ; y <= 1 ; y++)
//     {
//         for (int x = -1 ; x <= 1 ; x++)
//         {
//             vec2 offsets = vec2(float(x) * xOffset, float(y) * yOffset);
//             vec3 uvc = vec3(fs_in.shadowCoords.xy + offsets, fs_in.shadowCoords.z);

//             if (texture(shadowMap, uvc) >  0.f)
//                 factor += (fs_in.shadowCoords.w * 0.4f);
//            a++;
//         }
//     }
//     float value = (.5f + (factor / a));
//     if( value > 1.f )
//         value = 1.f ;

//     return value ;
// }

bool Is(float f)
{
    return f > .5f;
}

bool NormalMaping()
{
    float dist = length(perFrame.cameraPosition - fs_in.worldPos.xyz);
    return Is(perApp.useTextures.y) && (dist < perApp.viewDistance.y);
}

const vec2 thresholds = vec2(.9f, .3f);

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

vec3 getTriPlanarTextureBlendingFactor()
{
    vec3 normal = fs_in.faceNormal;
    vec3 blending = abs(normal);
    blending = normalize(max(blending, EPSILON));
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
    //blending.y = 0;
    return blending;
}

vec4 getTriPlanarMappingColor(sampler2D terrainTexture, vec3 worldCoordinates, float scale)
{
    vec4 xaxis = texture(terrainTexture, worldCoordinates.yz * scale);
    vec4 yaxis = texture(terrainTexture, worldCoordinates.xz * scale);
    vec4 zaxis = texture(terrainTexture, worldCoordinates.xy * scale);

    vec3 blending = getTriPlanarTextureBlendingFactor();

    return xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
}

vec4 calculateBackgroundColor(vec2 tiledCoords, float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor();
    vec4 backgorundTextureColor = getTriPlanarMappingColor(backgorundTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x) * backTextureAmount * (1.f - blendFactor);
    vec4 rockTextureColor       = getTriPlanarMappingColor(rockTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y) * backTextureAmount * blendFactor;
    return backgorundTextureColor + rockTextureColor;
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

    vec4 backgorundTextureColour = calculateBackgroundColor(tiledCoords, backTextureAmount);
    vec4 redTextureColor        = texture(redTexture, tiledCoords) * blendMapColor.r;
    vec4 greenTextureColor      = texture(greenTexture, tiledCoords) * blendMapColor.g;
    vec4 blueTextureColor       = texture(blueTexture, tiledCoords) * blendMapColor.b;
    vec4 alphaTextureColor      = texture(alphaTexture, tiledCoords) * blendMapColor.a;
    return backgorundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 calculateBackgroundNormal(vec2 tiledCoords, float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor();
    vec4 backgorundTextureColor = getTriPlanarMappingColor(backgorundTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x) * backTextureAmount * (1.f - blendFactor);
    vec4 rockTextureColor       = getTriPlanarMappingColor(rockTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y) * backTextureAmount * blendFactor;
    return backgorundTextureColor + rockTextureColor;
}

vec4 CalculateTerrainNormal(vec2 tiledCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (NormalMaping())
    {
        vec4 backgorundNormalColor   = calculateBackgroundNormal(tiledCoords, backTextureAmount);
        vec4 redNormalTextureColor   = texture(redTextureNormal, tiledCoords) * blendMapColor.r;
        vec4 greenNormalTextureColor = texture(greenTextureNormal, tiledCoords) * blendMapColor.g;
        vec4 blueNormalTextureColor  = texture(blueTextureNormal, tiledCoords) * blendMapColor.b;
        vec4 alphaNormalTextureColor = texture(alphaTextureNormal, tiledCoords) * blendMapColor.a;
        vec3 bumpNormal              = CalcBumpedNormal(backgorundNormalColor + redNormalTextureColor + greenNormalTextureColor + blueNormalTextureColor + alphaNormalTextureColor);
        return vec4(bumpNormal, 1.f); // w use fog
    }
    else
    {
       return vec4(normalize(fs_in.normal), 1.f);
    }
}

TerrainData GetTerrainData()
{
    vec2 tiledCoords   = fs_in.texCoord * 60.0f ;
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
    DiffuseOut      = vec4(terrainData.color.xyz * shadowFactor, terrainData.color.a);
    NormalOut       = terrainData.normal;
    SpecularOut     = vec4(0.f, 0.f, 0.f, 0.f);
}