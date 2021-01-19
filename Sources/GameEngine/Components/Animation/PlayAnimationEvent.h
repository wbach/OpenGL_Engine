#pragma once
#include <functional>
#include <vector>

#include "Common.h"
#include "Event.h"
#include "PlayDirection.h"
#include "AnimationPlayingInfo.h"

namespace GameEngine
{
namespace Components
{
struct PlayAnimationEvent : public Event
{
    PlayAnimationEvent(const AnimationPlayingInfo&);
    PlayAnimationEvent(const Animation::AnimationClip&);
    PlayAnimationEvent(const Animation::AnimationClip&, float, PlayDirection);
    PlayAnimationEvent(const Animation::AnimationClip&, float, PlayDirection, const std::vector<std::function<void()>>&);
    void process() override;

    AnimationPlayingInfo animationPlayingInfo_;
};
}  // namespace Components
}  // namespace GameEngine