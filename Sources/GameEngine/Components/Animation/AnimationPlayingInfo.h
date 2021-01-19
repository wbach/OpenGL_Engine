#pragma once

#include <functional>
#include <vector>

#include "GameEngine/Animations/AnimationClip.h"
#include "PlayDirection.h"

namespace GameEngine
{
namespace Components
{
struct AnimationPlayingInfo
{
    const Animation::AnimationClip& clip;
    float playSpeed_{1.f};
    PlayDirection playDirection_{PlayDirection::forward};
    const std::vector<std::function<void()>>& endCallbacks_;
};
}  // namespace Components
}  // namespace GameEngine