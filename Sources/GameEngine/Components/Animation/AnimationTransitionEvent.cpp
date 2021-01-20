#include "AnimationTransitionEvent.h"

#include "AnimationTransition.h"
#include "StateMachine.h"

#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
AnimationTransitionEvent::AnimationTransitionEvent(Pose& currentPose,
                                                   const AnimationPlayingInfo& nextAnimationClipInfo)
    : AnimationTransitionEvent(currentPose, nextAnimationClipInfo, 0.25f)
{
}
AnimationTransitionEvent::AnimationTransitionEvent(Pose& currentPose,
                                                   const AnimationPlayingInfo& nextAnimationClipInfo,
                                                   float timeForChange)
    : timeForChange{timeForChange}
    , currentPose{currentPose}
    , nextClipInfo{ nextAnimationClipInfo }
{
}
void AnimationTransitionEvent::process()
{
    machine->currentState_ = std::make_unique<AnimationTransition>(*this);
}
}  // namespace Components
}  // namespace GameEngine
