#version 440 core
#extension GL_GOOGLE_include_directive : enable

const vec3 normalizedDummySunVector = vec3(0.5773502691896258, 0.5773502691896258, 0.5773502691896258);
const float ambientFactor = 0.4f;
const vec2 defaultDiffRange = vec2(0.f, 1.f);
const vec2 diffRange = vec2(0.f, 1.f - ambientFactor);

#include "../../../Common/Common.glsl"
#include "../../../Common/PerApp.glsl"
#include "../../../Common/PerFrameBuffer.glsl"
#include "../../PerTerrainTexturesBuffer.glsl"
#include "../TerrainFragShaderCommon.glsl"

in VS_OUT
{
    vec4 worldPosition;
    vec2 texCoord;
    vec3 normal;
    float visibility;
} fs_in;

out vec4 outputColor;

vec4 calculateBackgroundColor(float backTextureAmount)
{
    if (abs(backTextureAmount) < EPSILON)
    {
        return vec4(0.f);
    }

    float blendFactor = GetRockBlendFactor(fs_in.normal);

    vec4 rockTextureColor       = DEFAULT_COLOR;
    vec4 backgroundTextureColor = DEFAULT_COLOR;

    if (Is(perTerrainTextures.haveTextureBackground.x))
    {
        backgroundTextureColor = getTriPlanarMappingColor(fs_in.normal, backgroundTexture, fs_in.worldPosition.xyz, perTerrainTextures.backgroundTextureScales.x);
    }

    if (Is(perTerrainTextures.haveTextureRock.x))
    {
        rockTextureColor = getTriPlanarMappingColor(fs_in.normal, rockTexture, fs_in.worldPosition.xyz, perTerrainTextures.backgroundTextureScales.y);
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
    vec4 redTextureColor         = textureColor(redTexture, textureCoords * perTerrainTextures.rgbaTextureScales.x, perTerrainTextures.haveTextureR.x, DEFAULT_COLOR) * blendMapColor.r;
    vec4 greenTextureColor       = textureColor(greenTexture, textureCoords * perTerrainTextures.rgbaTextureScales.y, perTerrainTextures.haveTextureG.x, DEFAULT_COLOR) * blendMapColor.g;
    vec4 blueTextureColor        = textureColor(blueTexture, textureCoords * perTerrainTextures.rgbaTextureScales.z, perTerrainTextures.haveTextureB.x, DEFAULT_COLOR) * blendMapColor.b;
    vec4 alphaTextureColor       = textureColor(alphaTexture, textureCoords * perTerrainTextures.rgbaTextureScales.w, perTerrainTextures.haveTextureA.x, DEFAULT_COLOR) * blendMapColor.a;

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
}