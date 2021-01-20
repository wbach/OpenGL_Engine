#include "PlayAnimationEvent.h"

#include <Logger/Log.h>

#include "PlayAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayAnimationEvent::PlayAnimationEvent(Pose& pose, const AnimationPlayingInfo& animationPlayingInfo)
    : currentPose{pose}
    , animationPlayingInfo_{animationPlayingInfo}
{
}
PlayAnimationEvent::PlayAnimationEvent(Pose& pose, const Animation::AnimationClip& clip)
    : PlayAnimationEvent(pose, clip, 1.f, PlayDirection::forward)
{
}

PlayAnimationEvent::PlayAnimationEvent(Pose& pose, const Animation::AnimationClip& clip, float playSpeed,
                                       PlayDirection playDirection)
    : PlayAnimationEvent(pose, clip, playSpeed, playDirection, {})
{
}
PlayAnimationEvent::PlayAnimationEvent(Pose& pose, const Animation::AnimationClip& clip, float playSpeed,
                                       PlayDirection playDirection,
                                       const std::vector<std::function<void()>>& endCallbacks)
    : currentPose{pose}
    , animationPlayingInfo_{clip, playSpeed, playDirection, endCallbacks}
{
}
void PlayAnimationEvent::process()
{
    machine->currentState_ = std::make_unique<PlayAnimation>(*this);
}
}  // namespace Components
}  // namespace GameEngine
