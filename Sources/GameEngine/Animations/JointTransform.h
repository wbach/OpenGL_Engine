#pragma once
#include "Types.h"

namespace GameEngine
{
namespace Animation
{
struct JointTransform
{
    vec3 position{0.f};
    Quaternion rotation{0.f, 0.f, 0.f, 0.f};
    vec3 scale{1.f};
};
}  // namespace Animation
}  // namespace GameEngine
