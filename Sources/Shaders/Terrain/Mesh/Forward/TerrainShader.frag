#version 330

in VS_OUT
{
    vec2 texCoord;
    vec3 normal;
} vs_in;

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
    float normalY = abs(normalize(vs_in.normal).y);
    normalY = normalY * 2;
    if (normalY > 1 )
        normalY = 1;
    return normalY;
}

vec4 CalculateTerrainColor()
{
    float normalY = GetScaledYNormal();
    vec4 blendMapColour     = texture(blendMap, vs_in.texCoord);
    float backTextureAmount = 1.f - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
    vec2 tiledCoords        = vs_in.texCoord * 40.0f ;
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