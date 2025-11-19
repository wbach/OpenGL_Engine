#version 430 core
#define MORPH_AREAS 8

layout (location = 0) in vec2 Position;

layout (std140, binding = 1) uniform PerFrame
{
    mat4 projectionViewMatrix;
    vec3 cameraPosition;
    vec4 clipPlane;
    vec4 projection;
} perFrame;

layout (std140, binding = 3) uniform PerTerrain
{
    vec4 displacementStrength;
    ivec4 morpharea1_4;
    ivec4 morpharea5_8;
    vec4 scaleAndOffset;
} perTerrain;

layout (std140, binding = 4) uniform PerNode
{
    mat4 worldMatrix;
    mat4 localMatrix;
    vec4 indexAndLocation; // xy index, zw location
    vec4 gapAndLod; // x gap, y Lod
} perNode;

layout(binding = 1) uniform sampler2D heightmap;

out vec2 mapCoord_TC;

float morphLatitude(vec2 position, vec2 location, vec2 index, float gap)
{
    vec2 frac  = position - location;

    if (index == vec2(0, 0))
    {
        float morph = frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    if (index == vec2(1, 0))
    {
        float morph = gap - frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    if (index == vec2(0, 1))
    {
        float morph = frac.x + frac.y - gap;
        if (morph > 0)
            return -morph;
    }
    if (index == vec2(1, 1))
    {
        float morph = frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    return 0;
}

float morphLongitude(vec2 position, vec2 location, vec2 index, float gap)
{
    vec2 frac = position - location;

    if (index == vec2(0, 0))
    {
        float morph = frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    if (index == vec2(1, 0))
    {
        float morph = frac.y - (gap - frac.x);
        if (morph > 0)
            return morph;
    }
    if (index == vec2(0, 1))
    {
        float morph = gap - frac.y - frac.x;
        if (morph > 0)
            return -morph;
    }
    if (index == vec2(1, 1))
    {
        float morph = frac.x - frac.y;
        if (morph > 0)
            return morph;
    }
    return 0;
}

vec2 morph(vec2 localPosition, int morphArea, vec3 scale){

    vec2 morphing = vec2(0, 0);
    vec2 location = perNode.indexAndLocation.zw;
    vec2 index    = perNode.indexAndLocation.xy;
    float gap     = perNode.gapAndLod.x;

    vec2 fixPointLatitude  = vec2(0, 0);
    vec2 fixPointLongitude = vec2(0, 0);
    float distLatitude     = 0.f;
    float distLongitude    = 0.f;

    if (index == vec2(0, 0))
    {
        fixPointLatitude  = location + vec2(gap, 0);
        fixPointLongitude = location + vec2(0, gap);
    }
    else if (index == vec2(1, 0))
    {
        fixPointLatitude  = location;
        fixPointLongitude = location + vec2(gap, gap);
    }
    else if (index == vec2(0, 1))
    {
        fixPointLatitude  = location + vec2(gap, gap);
        fixPointLongitude = location;
    }
    else if (index == vec2(1, 1))
    {
        fixPointLatitude  = location + vec2(0, gap);
        fixPointLongitude = location + vec2(gap, 0);
    }

    float planarFactor = 0.f;
    if (perFrame.cameraPosition.y > abs(scale.y))
    {
        planarFactor = 1;
    }
    else
    {
        planarFactor = perFrame.cameraPosition.y / abs(scale.y);
    }

    distLatitude = length(perFrame.cameraPosition - (perNode.worldMatrix *
                    vec4(fixPointLatitude.x, planarFactor, fixPointLatitude.y, 1)).xyz);
    distLongitude = length(perFrame.cameraPosition - (perNode.worldMatrix *
                    vec4(fixPointLongitude.x, planarFactor, fixPointLongitude.y, 1)).xyz);

    if (distLatitude > morphArea)
    {
        morphing.x = morphLatitude(localPosition.xy, location, index, gap);
    }

    if (distLongitude > morphArea)
    {
        morphing.y = morphLongitude(localPosition.xy, location, index, gap);
    }

    return morphing;
}

void main()
{
    vec2 localPosition = (perNode.localMatrix * vec4(Position.x, 0, Position.y, 1)).xz;

    int lodMorphArea[MORPH_AREAS];
    lodMorphArea[0] = perTerrain.morpharea1_4.x;
    lodMorphArea[1] = perTerrain.morpharea1_4.y;
    lodMorphArea[2] = perTerrain.morpharea1_4.z;
    lodMorphArea[3] = perTerrain.morpharea1_4.w;
    lodMorphArea[4] = perTerrain.morpharea5_8.x;
    lodMorphArea[5] = perTerrain.morpharea5_8.y;
    lodMorphArea[6] = perTerrain.morpharea5_8.z;
    lodMorphArea[7] = perTerrain.morpharea5_8.w;

    int lod = int(perNode.gapAndLod.y);

    if (lod > 0)
    {
        vec3 scale = perTerrain.scaleAndOffset.xyz;
        localPosition += morph(localPosition, lodMorphArea[lod - 1], scale);
    }

    mapCoord_TC = localPosition;
    float yOffset = perTerrain.scaleAndOffset.w;
    gl_Position = perNode.worldMatrix * vec4(localPosition.x, texture(heightmap, localPosition).r + yOffset, localPosition.y, 1);
}