#pragma once
#include <unordered_map>
#include "JointTransform.h"

namespace GameEngine
{
namespace Animation
{
struct KeyFrame
{
    float timeStamp{0.f};
    std::unordered_map<std::string, JointTransform> transforms;
};
}  // namespace Animation
}  // namespace GameEngine
