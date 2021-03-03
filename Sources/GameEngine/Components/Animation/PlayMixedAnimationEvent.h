#pragma once
#include <functional>
#include <vector>

#include "AnimationPlayingInfo.h"
#include "Common.h"
#include "Event.h"
#include "PlayDirection.h"

namespace GameEngine
{
namespace Components
{
struct PlayMixedAnimationEvent : public Event
{
    using JointsGroup = std::vector<uint32>;

    PlayMixedAnimationEvent(Pose&, const std::vector<std::pair<AnimationPlayingInfo, JointsGroup>>&);
    void process() override;

    Pose& currentPose;
    std::vector<std::pair<AnimationPlayingInfo, JointsGroup>> animationPlayingInfoPerGroup_;
};
}  // namespace Components
}  // namespace GameEngine
