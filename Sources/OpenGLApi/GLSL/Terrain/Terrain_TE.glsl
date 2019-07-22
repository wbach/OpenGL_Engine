#version 430
#define MORPH_AREAS 8

layout(quads, fractional_odd_spacing, cw) in;
layout (std140, binding = 3) uniform PerTerrain
{
    int lodMorphArea[MORPH_AREAS];
    float heightFactor;
} perTerrain;

uniform sampler2D heightmap;

in vec2 mapCoord_TE[];

out vec2 mapCoord_GS;

float GetHeight(vec2 mapCoord)
{
    float height = texture(heightmap, mapCoord).r;
    height *= perTerrain.heightFactor;
    height -= perTerrain.heightFactor / 2.f;
    height += 19.f;

    return height;
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