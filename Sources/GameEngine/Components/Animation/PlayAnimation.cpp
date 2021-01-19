#include "PlayAnimation.h"

#include "EndAnimationTransitionEvent.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayAnimation::PlayAnimation(const PlayAnimationEvent& event)
    : machine_{*event.machine}
    , clip_{event.animationPlayingInfo_.clip}
    , time_{0}
    , playingSpeed_{event.animationPlayingInfo_.playDirection_ == PlayDirection::forward
                        ? fabsf(event.animationPlayingInfo_.playSpeed_)
                        : -fabsf(event.animationPlayingInfo_.playSpeed_)}
    , endCallbacks_{event.animationPlayingInfo_.endCallbacks_}
{
}
Pose PlayAnimation::update(float deltaTime)
{
    auto currentPose = calculateCurrentAnimationPose(clip_, time_);
    increaseAnimationTime(deltaTime);
    return currentPose;
}
const std::string& PlayAnimation::getAnimationClipName() const
{
    return clip_.name;
}
void PlayAnimation::increaseAnimationTime(float deltaTime)
{
    time_ += deltaTime * playingSpeed_;

    if (time_ > clip_.GetLength())
    {
        if (clip_.playType == Animation::AnimationClip::PlayType::once)
        {
            machine_.handle(std::make_unique<EndAnimationTransitionEvent>(endCallbacks_));
            return;
        }
        time_ = fmodf(time_, clip_.GetLength());
    }
    if (time_ < 0)
    {
        time_ = clip_.GetLength() + time_;
    }
}
}  // namespace Components
}  // namespace GameEngine
