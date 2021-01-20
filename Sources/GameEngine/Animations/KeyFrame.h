#pragma once
#include <unordered_map>

#include "JointTransform.h"

namespace GameEngine
{
namespace Animation
{
using JointId = uint32;

struct KeyFrame
{
    float timeStamp{0.f};
    std::unordered_map<JointId, JointTransform> transforms;
};
}  // namespace Animation
}  // namespace GameEngine
