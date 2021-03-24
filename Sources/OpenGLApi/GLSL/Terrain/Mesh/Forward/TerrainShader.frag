#version 440 core
#include "../../PerTerrainTexturesBuffer.glsl"

const float EPSILON = 0.00001f;
const vec4 DEFAULT_COLOR         = vec4(0.8f, 0.8f, 0.8f, 1.f);

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
    vec4 fogData; // xyz - color, w - gradient
} perApp;

in GS_OUT
{
    vec4 worldPosition;
    vec2 texCoord;
    vec3 normal;
    vec3 faceNormal;
    float visibility;
} fs_in;

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
layout(binding = 19) uniform sampler2D rockTexture;
layout(binding = 20) uniform sampler2D rockTextureNormal;

out vec4 outputColor;

bool Is(float f)
{
    return f > .5f;
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

vec4 textureColor(sampler2D inputTexture, vec2 coords, float useThatTexture)
{
    if (Is(useThatTexture))
    {
        return texture(inputTexture, coords);
    }

    return DEFAULT_COLOR;
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

vec4 calculateBackgroundColor(float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor();

    vec4 rockTextureColor       = DEFAULT_COLOR;
    vec4 backgorundTextureColor = DEFAULT_COLOR;
    
    if (Is(perTerrainTextures.haveTextureBackground.x))
    {
        backgorundTextureColor = getTriPlanarMappingColor(backgorundTexture, fs_in.worldPosition.xyz, perTerrainTextures.backgroundTextureScales.x) * backTextureAmount;
    }

    if (Is(perTerrainTextures.haveTextureRock.x))
    {
        rockTextureColor = getTriPlanarMappingColor(rockTexture, fs_in.worldPosition.xyz, perTerrainTextures.backgroundTextureScales.y) * backTextureAmount;
    }

    return (backgorundTextureColor * (1.f - blendFactor)) + (rockTextureColor * blendFactor);
}

vec4 CalculateBackgroundColor(vec2 tiledCoords, float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor();
    vec4 backgorundTextureColor = texture(backgorundTexture, tiledCoords) * backTextureAmount * (1.f - blendFactor);
    vec4 rockTextureColor       = texture(rockTexture, tiledCoords) * backTextureAmount * blendFactor;
    return backgorundTextureColor + rockTextureColor;
}

vec4 CalculateTerrainColor(vec2 tiledCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!Is(perApp.useTextures.x))
    {
        return DEFAULT_COLOR;
    }

    vec4 backgorundTextureColour = calculateBackgroundColor(backTextureAmount);
    vec4 redTextureColor         = textureColor(redTexture, tiledCoords, perTerrainTextures.haveTextureR.x) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTexture, tiledCoords, perTerrainTextures.haveTextureG.x) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTexture, tiledCoords, perTerrainTextures.haveTextureB.x) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTexture, tiledCoords, perTerrainTextures.haveTextureA.x) * blendMapColor.a;

    return backgorundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 CalculateTerrainColor()
{
    vec2 tiledCoords        = fs_in.texCoord * 60.0f ;
    vec4 blendMapColor      = texture(blendMap, fs_in.texCoord);
    float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);
    return CalculateTerrainColor(tiledCoords, blendMapColor, backTextureAmount);
}

const vec3 normalizedDummySunVector = vec3(0.5773502691896258, 0.5773502691896258, 0.5773502691896258);
const float ambientFactor = 0.4f;
const vec2 defaultDiffRange = vec2(0.f, 1.f);
const vec2 diffRange = vec2(0.f, 1.f - ambientFactor);

float convertValueFromRange(float value, vec2 sourceRange, vec2 targetRange)
{
    float oldRange= sourceRange.y - sourceRange.x;
    float newRange = targetRange.y - targetRange.x;
    return (((value - sourceRange.x) * newRange) / oldRange) + targetRange.x;
}

void main()
{
    float dummyDiffuseFactor = dot(normalizedDummySunVector, fs_in.normal);
    if (dummyDiffuseFactor > 0)
    {
        float newDiffFactor = convertValueFromRange(dummyDiffuseFactor, defaultDiffRange, diffRange);
        dummyDiffuseFactor = newDiffFactor + ambientFactor;
    }
    else
    {
        dummyDiffuseFactor = ambientFactor;
    }

    vec4 color = CalculateTerrainColor();
    outputColor = vec4(color.rgb * dummyDiffuseFactor, color.a);

    const vec4 fogColor = vec4(0.8, 0.8, 0.8, 1.f);
    outputColor = mix(fogColor, outputColor, fs_in.visibility);
}