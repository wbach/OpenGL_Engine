#pragma once
#include <unordered_map>
#include "Animation.h"
#include "Types.h"

namespace GameEngine
{
namespace Collada
{
struct AnimationClip
{
    std::unordered_map<std::string, Animation> animations_;
};

struct LibraryAnimations
{
    std::unordered_map<std::string, AnimationClip> animationsClips_;
};
}
}
