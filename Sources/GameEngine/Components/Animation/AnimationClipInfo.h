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
    struct Subscription
    {
        IdType id;
        std::function<void()> callback;
        float timeStamp;
    };
    float playSpeed{1.f};
    Animation::AnimationClip clip;
    PlayPolicy policy{PlayPolicy::PlayInLoop};
    PlayDirection playDirection{PlayDirection::forward};
    std::vector<Subscription> subscribers;
};
}  // namespace Components
}  // namespace GameEngine
