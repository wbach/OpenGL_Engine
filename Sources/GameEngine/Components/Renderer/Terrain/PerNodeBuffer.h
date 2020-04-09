#pragma once
#include <Types.h>
#include <Utils/GLM/GLMUtils.h>

namespace GameEngine
{
struct PerNode
{
    mat4 worldMatrix = mat4(1.f);
    mat4 localMatrix = mat4(1.f);
    vec4 indexAndLocation = vec4(0);  // xy index, zw location
    vec4 gapAndLod = vec4(0);         // x gap, y Lod

    PerNode(const vec2& index, const vec2& location, int lod, const vec3& worldPosition, const vec3& scale,
            float terrainRootNodesCount)
        : indexAndLocation(index.x, index.y, location.x, location.y)
        , gapAndLod(1.f / (terrainRootNodesCount * powf(2.f, static_cast<float>(lod))),
                    static_cast<float>(lod), 0, 0)
    {
        localMatrix = Utils::CreateTransformationMatrix(vec3(indexAndLocation.z, 0, indexAndLocation.w), DegreesVec3(0),
                                                        vec3(gapAndLod.x, 0, gapAndLod.x));
        worldMatrix = Utils::CreateTransformationMatrix(worldPosition, DegreesVec3(0), scale);
    }
};

}  // namespace GameEngine
