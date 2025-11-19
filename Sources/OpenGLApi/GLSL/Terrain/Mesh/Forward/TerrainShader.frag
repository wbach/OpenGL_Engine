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
layout(binding = 4) uniform sampler2D backgroundTexture;
layout(binding = 5) uniform sampler2D backgroundTextureNormal;
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
    float value = 1.f - abs(fs_in.normal.y);

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
    vec4 backgroundTextureColor = DEFAULT_COLOR;

    if (Is(perTerrainTextures.haveTextureBackground.x))
    {
        backgroundTextureColor = getTriPlanarMappingColor(backgroundTexture, fs_in.worldPosition.xyz, perTerrainTextures.backgroundTextureScales.x);
    }

    if (Is(perTerrainTextures.haveTextureRock.x))
    {
        rockTextureColor = getTriPlanarMappingColor(rockTexture, fs_in.worldPosition.xyz, perTerrainTextures.backgroundTextureScales.y);
    }

    return ((backgroundTextureColor * (1.f - blendFactor)) + (rockTextureColor * blendFactor)) * backTextureAmount;
}

vec4 CalculateTerrainColor(vec2 textureCoords, vec4 blendMapColor, float backTextureAmount)
{
    if (!Is(perApp.useTextures.x))
    {
        return DEFAULT_COLOR;
    }

    vec4 backgroundTextureColour = calculateBackgroundColor(backTextureAmount);
    vec4 redTextureColor         = textureColor(redTexture, textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.x) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTexture, textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.x) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTexture, textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.x) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTexture, textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.x) * blendMapColor.a;

    return backgroundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 CalculateTerrainColor()
{
    vec4 blendMapColor      = vec4(0);
    if (perTerrainTextures.haveBlendMap > 0.5f)
    {
        blendMapColor = texture(blendMap, fs_in.texCoord);
    }
    float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);
    return CalculateTerrainColor(fs_in.texCoord, blendMapColor, backTextureAmount);
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

    // const vec4 fogColor = vec4(0.8, 0.8, 0.8, 1.f);
    // outputColor = mix(fogColor, outputColor, fs_in.visibility);

     //outputColor = texture(blendMap, fs_in.texCoord);
}