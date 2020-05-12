#version 440 core

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
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

out vec4 outputColor;

vec4 CalculateTerrainColor(vec2 tiledCoords, vec4 blendMapColor, float backTextureAmount)
{
    vec4 backgorundTextureColour = texture(backgorundTexture, tiledCoords) * backTextureAmount;
    vec4 redTextureColor        = texture(redTexture, tiledCoords) * blendMapColor.r;
    vec4 greenTextureColor      = texture(greenTexture, tiledCoords) * blendMapColor.g;
    vec4 blueTextureColor       = texture(blueTexture, tiledCoords) * blendMapColor.b;
    vec4 alphaTextureColor      = texture(alphaTexture, tiledCoords) * blendMapColor.a;
    return backgorundTextureColour + redTextureColor + greenTextureColor + blueTextureColor + alphaTextureColor;
}

vec4 CalculateTerrainColor()
{
    vec2 tiledCoords        = vs_in.texCoord * 40.0f ;
    vec4 blendMapColor = texture(blendMap, vs_in.texCoord);
    float backTextureAmount = 1.f - (blendMapColor.r + blendMapColor.g + blendMapColor.b + blendMapColor.a);
    return CalculateTerrainColor(tiledCoords, blendMapColor, backTextureAmount);
}

void main()
{
    outputColor = CalculateTerrainColor();
}