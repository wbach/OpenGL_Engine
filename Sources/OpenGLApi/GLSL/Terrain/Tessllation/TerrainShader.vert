#version 430
#define MORPH_AREAS 8

layout (location = 0) in vec2 Position;

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    mat4 toShadowMapSpace;
    vec3 cameraPosition;
} perFrame;

layout (std140, binding = 3) uniform PerTerrain
{
    int lodMorphArea[MORPH_AREAS];
    float heightFactor; // sacaleY
} perTerrain;

layout (std140, binding = 4) uniform PerNode
{
    mat4 worldMatrix;
    mat4 localMatrix;
    vec2 index;
    vec2 location;
    float gap;
    int lod;
} perNode;

uniform sampler2D heightmap;

out vec2 mapCoord_TC;

float morphLatitude(vec2 position)
{
    vec2 frac = position - perNode.location;

    if (perNode.index == vec2(0, 0))
    {
        float morph = frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    if (perNode.index == vec2(1, 0))
    {
        float morph = perNode.gap - frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    if (perNode.index == vec2(0, 1))
    {
        float morph = frac.x + frac.y - perNode.gap;
        if (morph > 0)
            return -morph;
    }
    if (perNode.index == vec2(1, 1))
    {
        float morph = frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    return 0;
}

float morphLongitude(vec2 position)
{
    vec2 frac = position - perNode.location;

    if (perNode.index == vec2(0, 0))
    {
        float morph = frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    if (perNode.index == vec2(1, 0))
    {
        float morph = frac.y - (perNode.gap - frac.x);
        if (morph > 0)
            return morph;
    }
    if (perNode.index == vec2(0, 1))
    {
        float morph = perNode.gap - frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    if (perNode.index == vec2(1, 1))
    {
        float morph = frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    return 0;
}

vec2 morph(vec2 localPosition, int morphArea){

    vec2 morphing = vec2(0, 0);

    vec2 fixPointLatitude  = vec2(0, 0);
    vec2 fixPointLongitude = vec2(0, 0);
    float distLatitude     = 0.f;
    float distLongitude    = 0.f;

    if (perNode.index == vec2(0, 0))
    {
        fixPointLatitude  = perNode.location + vec2(perNode.gap, 0);
        fixPointLongitude = perNode.location + vec2(0, perNode.gap);
    }
    else if (perNode.index == vec2(1, 0))
    {
        fixPointLatitude  = perNode.location;
        fixPointLongitude = perNode.location + vec2(perNode.gap, perNode.gap);
    }
    else if (perNode.index == vec2(0, 1))
    {
        fixPointLatitude  = perNode.location + vec2(perNode.gap, perNode.gap);
        fixPointLongitude = perNode.location;
    }
    else if (perNode.index == vec2(1, 1))
    {
        fixPointLatitude  = perNode.location + vec2(0, perNode.gap);
        fixPointLongitude = perNode.location + vec2(perNode.gap, 0);
    }

    float planarFactor = 0.f;
    if (perFrame.cameraPosition.y > abs(perTerrain.heightFactor))
    {
        planarFactor = 1;
    }
    else
    {
        planarFactor = perFrame.cameraPosition.y / abs(perTerrain.heightFactor);
    }

    distLatitude = length(perFrame.cameraPosition - (perNode.worldMatrix *
                    vec4(fixPointLatitude.x, planarFactor, fixPointLatitude.y, 1)).xyz);
    distLongitude = length(perFrame.cameraPosition - (perNode.worldMatrix *
                    vec4(fixPointLongitude.x, planarFactor, fixPointLongitude.y, 1)).xyz);

    if (distLatitude > morphArea)
    {
        morphing.x = morphLatitude(localPosition.xy);
    }

    if (distLongitude > morphArea)
    {
        morphing.y = morphLongitude(localPosition.xy);
    }

    return morphing;
}

void main()
{
    vec2 localPosition = (perNode.localMatrix * vec4(Position.x, 0, Position.y, 1)).xz;

    if (perNode.lod > 0)
    {
        localPosition += morph(localPosition, perTerrain.lodMorphArea[perNode.lod - 1]);
    }

    mapCoord_TC = localPosition;
    gl_Position = perNode.worldMatrix * vec4(localPosition.x, texture(heightmap, localPosition).r, localPosition.y, 1);
}