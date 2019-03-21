#version 430

layout (location = 0) out vec4 WorldPosOut;
layout (location = 1) out vec4 DiffuseOut;
layout (location = 2) out vec4 NormalOut;
layout (location = 3) out vec4 SpecularOut;

uniform sampler2D normalmap;
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

in vec2 mapCoord_FS;
in vec4 worldPos;
in vec3 normal_FS;


vec4 CalculateColor(sampler2D textureId, float factor, vec2 textCoords, float normalFactor)
{
    vec4 baseColor = texture(textureId, textCoords) * factor * normalFactor;
    vec4 rockColor = texture(rockTexture, textCoords) * factor * (1.f - normalFactor);

    return  baseColor + rockColor;
}

vec4 CalculateTerrainColor()
{
    vec4 blend_map_colour = texture(blendMap, mapCoord_FS) ;

    float back_texture_amount = 1 - (blend_map_colour.r + blend_map_colour.g + blend_map_colour.b) ;
    vec2 tiled_coords = mapCoord_FS * 200.0f ;

    float normalFactor = dot(normal_FS, vec3(0.f, 4.f, 0.f));

    if (normalFactor > 1 )
     normalFactor = 1;

    vec4 backgorund_texture_colour;
    backgorund_texture_colour = CalculateColor(backgorundTexture, back_texture_amount, tiled_coords, normalFactor);

    vec4 r_texture_colour = CalculateColor(redTexture, blend_map_colour.r, tiled_coords, normalFactor);
    vec4 g_texture_colour = CalculateColor(greenTexture, blend_map_colour.g, tiled_coords, normalFactor);
    vec4 b_texture_colour = CalculateColor(blueTexture, blend_map_colour.b, tiled_coords, normalFactor);

    return backgorund_texture_colour + r_texture_colour + g_texture_colour + b_texture_colour ;
}

void main()
{
    WorldPosOut = worldPos;
    //DiffuseOut  = texture(normalmap, mapCoord_FS);
    DiffuseOut =  vec4(CalculateTerrainColor().xyz,1.0);
    NormalOut = vec4(normalize(texture(normalmap, mapCoord_FS).xyz), 1.f);
    //NormalOut   = vec4(0,1,0, 1);
    SpecularOut = vec4(0);
}