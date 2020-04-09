#pragma once
#include <unordered_map>
#include "JointTransform.h"

namespace GameEngine
{
namespace Animation
{
struct KeyFrame
{
    float timeStamp;
    std::unordered_map<std::string, JointTransform> transforms;
    std::unordered_map<uint32, JointTransform> idTransforms_;
};
}  // namespace Animation
}  // namespace GameEngine
