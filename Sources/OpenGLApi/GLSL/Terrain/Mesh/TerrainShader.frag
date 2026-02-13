#version 440 core
#extension GL_GOOGLE_include_directive : enable
#include "../PerTerrainTexturesBuffer.glsl"

const float EPSILON              = 0.00001f;
const vec4 DEFAULT_COLOR         = vec4(0.8f, 0.8f, 0.8f, 1.f);
const vec4 DEFAULT_NORMAL_COLOR  = vec4(0.5f, 0.5f, 1.0f, 1.0f); // tangent space vec4(0,0,1,1)
const vec2 thresholds            = vec2(.9f, .3f);

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
    mat3 tbn;
    vec3 faceNormal;
    float visibility;
} fs_in;

layout(binding = 0) uniform sampler2D blendMap;
// --- Background (Warstwa bazowa) ---
layout(binding = 1) uniform sampler2D backgroundTexture;
layout(binding = 2) uniform sampler2D backgroundTextureNormal;
layout(binding = 3) uniform sampler2D backgroundTextureDisplacement;

// --- Kanał Czerwony (R) ---
layout(binding = 4) uniform sampler2D redTexture;
layout(binding = 5) uniform sampler2D redTextureNormal;
layout(binding = 6) uniform sampler2D redTextureDisplacement;

// --- Kanał Zielony (G) ---
layout(binding = 7) uniform sampler2D greenTexture;
layout(binding = 8) uniform sampler2D greenTextureNormal;
layout(binding = 9) uniform sampler2D greenTextureDisplacement;

// --- Kanał Niebieski (B) ---
layout(binding = 10) uniform sampler2D blueTexture;
layout(binding = 11) uniform sampler2D blueTextureNormal;
layout(binding = 12) uniform sampler2D blueTextureDisplacement;

// --- Kanał Alfa (A) ---
layout(binding = 13) uniform sampler2D alphaTexture;
layout(binding = 14) uniform sampler2D alphaTextureNormal;
layout(binding = 15) uniform sampler2D alphaTextureDisplacement;

// --- Warstwa Skał (Slope/Rock) ---
layout(binding = 16) uniform sampler2D rockTexture;
layout(binding = 17) uniform sampler2D rockTextureNormal;
layout(binding = 18) uniform sampler2D rockTextureDisplacement;

layout(binding = 19) uniform sampler2D heightmap;
layout(binding = 20) uniform sampler2D normalmap;

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SurfaceParamsOut;

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

layout (std140, align=16, binding=1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
    vec4 time;
} perFrame;

vec3 CalcBumpedNormal(vec4 normalMapColor)
{
    vec3 bumpMapNormal = normalize(normalMapColor.rgb * 2.f - 1.f);
    return fs_in.tbn * bumpMapNormal;
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

vec4 textureColor(sampler2D baseTexture, sampler2D ambientOcclusionTexture, vec2 coords, vec2 useThatTexture)
{
    vec4 result = DEFAULT_COLOR;
    if (Is(useThatTexture.x))
    {
        result = texture(baseTexture, coords);
    }

    if (Is(useThatTexture.y))
    {
       result.w = texture(ambientOcclusionTexture, coords).r;
    }

    return result;
}

vec4 normalColor(sampler2D inputTexture, vec2 coords, float useThatTexture)
{
    if (Is(useThatTexture))
    {
        return texture(inputTexture, coords);
    }
    return DEFAULT_NORMAL_COLOR;
}

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
    vec3 blending = pow(abs(fs_in.faceNormal), vec3(8.0));
    blending /= (blending.x + blending.y + blending.z);

    //vec3 blending = getTriPlanarTextureBlendingFactor();

    vec4 xaxis = texture(terrainTexture, worldCoordinates.yz * scale);
    vec4 yaxis = texture(terrainTexture, worldCoordinates.xz * scale);
    vec4 zaxis = texture(terrainTexture, worldCoordinates.xy * scale);

    return xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;
}

vec4 calculateBackgroundColor(float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor();

    vec4 rockTextureColor       = DEFAULT_COLOR;
    vec4 backgroundTextureColor = DEFAULT_COLOR;

    if (Is(perApp.useTextures.x) && Is(perTerrainTextures.haveTextureBackground.x))
    {
        backgroundTextureColor = getTriPlanarMappingColor(backgroundTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x);
    }

    if (Is(perApp.useTextures.w) && Is(perTerrainTextures.haveTextureBackground.z))
    {
        backgroundTextureColor.w = getTriPlanarMappingColor(backgroundTextureDisplacement, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x).r;
    }

    if (Is(perApp.useTextures.x) &&  Is(perTerrainTextures.haveTextureRock.x))
    {
        rockTextureColor = getTriPlanarMappingColor(rockTexture, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y);
    }

    if (Is(perApp.useTextures.w) && Is(perTerrainTextures.haveTextureRock.z))
    {
        rockTextureColor.w = getTriPlanarMappingColor(rockTextureDisplacement, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y).r;
    }

    return ((backgroundTextureColor * (1.f - blendFactor)) + (rockTextureColor * blendFactor)) * backTextureAmount;
}

vec4 CalculateTerrainColor(vec2 textureCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!Is(perApp.useTextures.x) && !Is(perApp.useTextures.w))
    {
        return DEFAULT_COLOR;
    }

    vec4 backgroundTextureColour = calculateBackgroundColor(backTextureAmount);
    vec4 redTextureColor         = textureColor(redTexture, redTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.xz) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTexture, greenTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.xz) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTexture, blueTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.xz) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTexture, alphaTextureDisplacement, textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.xz) * blendMapColor.a;

    return backgroundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 calculateBackgroundNormal(float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor();

    vec4 backgroundTextureColor = DEFAULT_NORMAL_COLOR;
    vec4 rockTextureColor = DEFAULT_NORMAL_COLOR;


    if (Is(perTerrainTextures.haveTextureBackground.y))
    {
        backgroundTextureColor = getTriPlanarMappingColor(backgroundTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.x) * (1.f - blendFactor);
    }

    if (Is(perTerrainTextures.haveTextureRock.y))
    {
        rockTextureColor = getTriPlanarMappingColor(rockTextureNormal, fs_in.worldPos.xyz, perTerrainTextures.backgroundTextureScales.y) * blendFactor;
    }

    return ((backgroundTextureColor * (1.f - blendFactor)) + (rockTextureColor * blendFactor)) * backTextureAmount;
}

vec4 CalculateTerrainNormal(vec2 textureCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!NormalMaping())
    {
        return vec4(normalize(fs_in.normal), 1.f);
    }

    vec4 backgorundNormalColor   = calculateBackgroundNormal(backTextureAmount);
    vec4 redNormalTextureColor   = normalColor(redTextureNormal,  textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.y) * blendMapColor.r;
    vec4 greenNormalTextureColor = normalColor(greenTextureNormal,  textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.y) * blendMapColor.g;
    vec4 blueNormalTextureColor  = normalColor(blueTextureNormal,  textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.y) * blendMapColor.b;
    vec4 alphaNormalTextureColor = normalColor(alphaTextureNormal,  textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.y) * blendMapColor.a;
    vec3 normal                  = CalcBumpedNormal(backgorundNormalColor + redNormalTextureColor + greenNormalTextureColor + blueNormalTextureColor + alphaNormalTextureColor);
    return vec4(normal, 1.f); // w use fog
}

TerrainData GetTerrainData()
{
    vec4 blendMapColor = vec4(0.f);

    if (Is(perTerrainTextures.haveBlendMap))
    {
        blendMapColor = texture(blendMap, fs_in.texCoord);
    }

    float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);

    TerrainData result;
    result.color  = CalculateTerrainColor(fs_in.texCoord, blendMapColor, backTextureAmount);
    result.normal = CalculateTerrainNormal(fs_in.texCoord, blendMapColor, backTextureAmount);
    return result;
}

void main()
{
    TerrainData terrainData = GetTerrainData();
    const float metallic = 0.f;
    const float roughness = 0.9f;

    WorldPosOut      = fs_in.worldPos;
    DiffuseOut       = terrainData.color;
    NormalOut        = terrainData.normal;
    SurfaceParamsOut = vec4(metallic, roughness, 0.f, 0.f);
}