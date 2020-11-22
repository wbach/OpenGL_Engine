#version 440 core

layout (std140, align=16, binding=0) uniform PerApp
{
    vec4 useTextures; // x - diffuse, y - normalMap, z - specular, w - displacement
    vec4 viewDistance; // x - objectView, y - normalMapping, z - plants, w - trees
    vec4 shadowVariables;
} perApp;

in TES_OUT
{
    vec2 textCoord;
    vec3 normal;
    float height;
} fs_in;

uniform sampler2DShadow shadowMap;
uniform sampler2D blendMap;
uniform sampler2D backgorundTexture;
uniform sampler2D backgorundTextureNormal;
uniform sampler2D redTexture;
uniform sampler2D redTextureNormal;
uniform sampler2D greenTexture;
uniform sampler2D greenTextureNormal;
uniform sampler2D blueTexture;
uniform sampler2D blueTextureNormal;
uniform sampler2D rockTexture;
uniform sampler2D rockNormalTexture;
uniform sampler2D snowTexture;
uniform sampler2D snowTextureNormal;

const float EPSILON = 0.005f;

out vec4 outputColor;

vec4 CalculateColor(sampler2D textureId, float factor, vec2 textCoords, float normalFactor)
{
    vec4 baseColor = texture(textureId, textCoords) * factor * normalFactor;
    vec4 rockColor = texture(rockTexture, textCoords) * factor * (1.f - normalFactor);

    return  baseColor + rockColor;
}

vec4 CalculateTerrainColor()
{
    vec4 blend_map_colour = texture(blendMap, fs_in.textCoord) ;

    float back_texture_amount = 1 - (blend_map_colour.r + blend_map_colour.g + blend_map_colour.b) ;
    vec2 tiled_coords = fs_in.textCoord * 200.0f ;

    float normalFactor = dot(fs_in.normal, vec3(0.f, 4.f, 0.f));

    if (normalFactor > 1 )
       normalFactor = 1;

    vec4 backgorund_texture_colour;
    backgorund_texture_colour = CalculateColor(backgorundTexture, back_texture_amount, tiled_coords, normalFactor);

    vec4 r_texture_colour = CalculateColor(redTexture, blend_map_colour.r, tiled_coords, normalFactor);
    vec4 g_texture_colour = CalculateColor(greenTexture, blend_map_colour.g, tiled_coords, normalFactor);
    vec4 b_texture_colour = CalculateColor(blueTexture, blend_map_colour.b, tiled_coords, normalFactor);

    return backgorund_texture_colour + r_texture_colour + g_texture_colour + b_texture_colour ;
}

bool OutOfHeightMapSize()
{
    return (fs_in.textCoord.x < EPSILON || fs_in.textCoord.x > (1.f - EPSILON) || fs_in.textCoord.y <EPSILON || fs_in.textCoord.y > (1.f - EPSILON));
}

void main()
{
    if (OutOfHeightMapSize())
        discard;

    outputColor      = CalculateTerrainColor();
}
