#pragma once
#include "Types.h"
#include <array>

namespace GameEngine
{
const int MAX_BONES = 512;

struct PerPoseUpdate
{
    std::array<mat4, MAX_BONES> bonesTransforms;

    PerPoseUpdate()
    {
        for(auto& t : bonesTransforms)
            t = mat4(1.f);
    }
};
}  // namespace GameEngine
