#version 130

in vec2 texCoord;
in vec3 normal;

uniform sampler2D blendMap;
uniform sampler2D backgorundTexture;
uniform sampler2D redTexture;
uniform sampler2D greenTexture;
uniform sampler2D blueTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler2DShadow shadowMap;

out vec4 outputColor;

float GetScaledYNormal()
{
    float normalY = abs(normalize(normal).y);
    normalY = normalY * 2;
    if (normalY > 1 )
        normalY = 1;
    return normalY;
}

vec4 CalculateTerrainColor()
{
    float normalY = GetScaledYNormal();
    vec4 blendMapColour     = texture(blendMap, texCoord);
    float backTextureAmount = 1.f - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
    vec2 tiledCoords        = texCoord * 40.0f ;
    vec4 backgorundTextureColour = texture(backgorundTexture, tiledCoords) * backTextureAmount * normalY + ( texture(rockTexture, tiledCoords * 0.25f) * backTextureAmount * (1.f - normalY));
    vec4 redTextureColour        = texture(redTexture, tiledCoords) * blendMapColour.r;
    vec4 greenTextureColour      = texture(greenTexture, tiledCoords) * blendMapColour.g;
    vec4 blueTextureColour       = texture(blueTexture, tiledCoords) * blendMapColour.b;

    return backgorundTextureColour + redTextureColour + greenTextureColour + blueTextureColour;
}

void main()
{
    outputColor = CalculateTerrainColor();
}