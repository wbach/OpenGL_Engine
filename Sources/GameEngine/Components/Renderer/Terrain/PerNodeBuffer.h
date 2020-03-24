#pragma once
#include <Types.h>
#include <Utils/GLM/GLMUtils.h>

namespace GameEngine
{
struct PerNode
{
    mat4 worldMatrix;
    mat4 localMatrix;
    vec4 indexAndLocation;  // xy index, zw location
    vec4 gapAndLod;         // x gap, y Lod

    PerNode(const vec2& index, const vec2& location, int lod, const vec3& worldPosition, const vec3& scale,
            float terrainRootNodesCount)
        : indexAndLocation(index.x, index.y, location.x, location.y)
        , gapAndLod(1.f / (terrainRootNodesCount * powf(2.f, static_cast<float>(lod))),
                    static_cast<float>(lod), 0, 0)
    {
        localMatrix = Utils::CreateTransformationMatrix(vec3(indexAndLocation.z, 0, indexAndLocation.w), vec3(0),
                                                        vec3(gapAndLod.x, 0, gapAndLod.x));
        worldMatrix = Utils::CreateTransformationMatrix(worldPosition, vec3(0), scale);
    }
};

}  // namespace GameEngine
