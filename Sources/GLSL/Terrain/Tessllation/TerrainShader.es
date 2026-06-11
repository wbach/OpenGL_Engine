#version 450 core
#define MORPH_AREAS 8

layout(quads, fractional_odd_spacing, cw) in;
layout (std140, binding = 3) uniform PerTerrain
{
    vec4 displacementStrength;
    ivec4 morpharea1_4;
    ivec4 morpharea5_8;
    vec4 scaleAndOffset;
} perTerrain;

layout(binding = 1) uniform sampler2D heightmap;

in vec2 mapCoord_TE[];

out vec2 mapCoord_GS;

float GetHeight(vec2 mapCoord)
{
    float offset_ = perTerrain.scaleAndOffset.w;
    float heightFactor = perTerrain.scaleAndOffset.y;

    float height = texture(heightmap, mapCoord).r;
    return (height * heightFactor) - offset_;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // world position
    vec4 position =
    ((1 - u) * (1 - v) * gl_in[12].gl_Position +
    u * (1 - v) * gl_in[0].gl_Position +
    u * v * gl_in[3].gl_Position +
    (1 - u) * v * gl_in[15].gl_Position);

    vec2 mapCoord =
    ((1 - u) * (1 - v) * mapCoord_TE[12] +
    u * (1 - v) * mapCoord_TE[0] +
    u * v * mapCoord_TE[3] +
    (1 - u) * v * mapCoord_TE[15]);

    position.y  = GetHeight(mapCoord);
    mapCoord_GS = mapCoord;
    gl_Position = position;
}