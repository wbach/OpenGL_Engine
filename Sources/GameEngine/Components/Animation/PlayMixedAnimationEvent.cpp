#include "PlayMixedAnimationEvent.h"
#include "PlayMixedAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayMixedAnimationEvent::PlayMixedAnimationEvent(Pose& pose,
                                                 const std::vector<std::pair<AnimationPlayingInfo, JointsGroup>>& groups)
    : currentPose{pose}
    , animationPlayingInfoPerGroup_{groups}
{
}
void PlayMixedAnimationEvent::process()
{
    machine->currentState_ = std::make_unique<PlayMixedAnimation>(*this);
}
}  // namespace Components
}  // namespace GameEngine
