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
    AnimationTransitionEvent(Pose&, const AnimationPlayingInfo&);
    AnimationTransitionEvent(Pose&, const AnimationPlayingInfo&, float);
    void process() override;

    Pose& currentPose;
    float timeForChange;
    AnimationPlayingInfo nextClipInfo;
};

}  // namespace Components
}  // namespace GameEngine
