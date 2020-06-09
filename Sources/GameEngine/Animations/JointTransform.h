#pragma once
#include "GLM/GLMUtils.h"
#include "Types.h"

namespace GameEngine
{
namespace Animation
{
struct JointTransform
{
    vec3 position{0.f};
    Quaternion rotation{0.f, 0.f, 0.f, 0.f};
};
}  // namespace Animation
}  // namespace GameEngine
