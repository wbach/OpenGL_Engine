#include "AnimationTransition.h"
#include "StateMachine.h"
#include "AnimationTransitionEvent.h"
#include "PlayAnimationEvent.h"

namespace GameEngine
{
namespace Components
{
AnimationTransition::AnimationTransition(const AnimationTransitionEvent& event)
    : machine_{*event.machine}
    , info_{event.nextClipInfo}
    , startChaneAnimKeyFrame_{convert(event.currentPose)}
    , endChangeAnimKeyFrame_{not event.nextClipInfo.clip.GetFrames().empty()
                                 ? event.nextClipInfo.clip.GetFrames().front()
                                 : Animation::KeyFrame{}}
    , timeForChange_{event.timeForChange}
    , currentTime_{0.f}
{
}
Pose AnimationTransition::update(float deltaTime)
{
    auto pose = interpolatePoses(startChaneAnimKeyFrame_, endChangeAnimKeyFrame_, currentTime_);
    calculateTime(deltaTime);
    return pose;
}
const std::string& AnimationTransition::getAnimationClipName() const
{
    return info_.clip.name;
}
void AnimationTransition::calculateTime(float deltaTime)
{
    currentTime_ += (1.f / timeForChange_) * deltaTime;
    if (currentTime_ > 1.f)
    {
        machine_.handle(std::make_unique<PlayAnimationEvent>(info_));
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
