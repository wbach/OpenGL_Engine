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

out vec4 outputColor;

vec4 CalculateTerrainColor()
{
    vec4 blendMapColour     = texture(blendMap, vs_in.texCoord);
    float backTextureAmount = 1.f - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
    vec2 tiledCoords        = vs_in.texCoord * 40.0f ;
    vec4 backgorundTextureColour = texture(backgorundTexture, tiledCoords) * backTextureAmount;
    vec4 redTextureColour        = texture(redTexture, tiledCoords) * blendMapColour.r;
    vec4 greenTextureColour      = texture(greenTexture, tiledCoords) * blendMapColour.g;
    vec4 blueTextureColour       = texture(blueTexture, tiledCoords) * blendMapColour.b;

    return backgorundTextureColour + redTextureColour + greenTextureColour + blueTextureColour;
}

void main()
{
    outputColor = CalculateTerrainColor();
}