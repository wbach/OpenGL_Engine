#pragma once
#include "Types.h"

namespace GameEngine
{
const int MAX_BONES = 512;

struct PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];

    PerPoseUpdate()
    {
        for(auto& t : bonesTransforms)
            t = mat4(1.f);
    }
};
}  // namespace GameEngine
