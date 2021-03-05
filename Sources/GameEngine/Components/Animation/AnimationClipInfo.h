#pragma once
#include <functional>
#include <vector>

#include "GameEngine/Animations/AnimationClip.h"
#include "PlayDirection.h"
#include "PlayPolicy.h"

namespace GameEngine
{
namespace Components
{
struct AnimationClipInfo
{
    float playSpeed{1.f};
    const Animation::AnimationClip& clip;
    PlayPolicy policy{PlayPolicy::PlayInLoop};
    PlayDirection playDirection{PlayDirection::forward};
    const std::vector<std::function<void()>>& endCallbacks_;
};
}  // namespace Components
}  // namespace GameEngine
