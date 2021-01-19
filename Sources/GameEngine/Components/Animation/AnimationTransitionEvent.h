#pragma once
#include "Common.h"
#include "Event.h"
#include "AnimationPlayingInfo.h"

namespace GameEngine
{
namespace Components
{
struct AnimationTransitionEvent : public Event
{
    AnimationTransitionEvent(const Pose&, const AnimationPlayingInfo&);
    AnimationTransitionEvent(const Pose&, const AnimationPlayingInfo&, float);
    void process() override;

    float timeForChange;
    const Pose& currentPose;

    AnimationPlayingInfo nextClipInfo;
};

}  // namespace Components
}  // namespace GameEngine
