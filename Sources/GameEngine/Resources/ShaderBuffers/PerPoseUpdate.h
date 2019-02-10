#pragma once
#include "Types.h"

namespace GameEngine
{
const int MAX_BONES = 25;

struct PerPoseUpdate
{
    mat4 bonesTransforms[MAX_BONES];
};
}  // namespace GameEngine
