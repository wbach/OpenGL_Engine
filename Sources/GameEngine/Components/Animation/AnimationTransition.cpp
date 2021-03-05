#include "AnimationTransition.h"

#include "PlayAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
AnimationTransition::AnimationTransition(Context& context, float /*startTime*/, const AnimationClipInfo& infoClip)
    : context_{context}
    , info_{infoClip}
    , startChaneAnimKeyFrame_{convert(context_.currentPose)}  // to do , calculate pose for startTime
    , endChangeAnimKeyFrame_{not infoClip.clip.GetFrames().empty() ? infoClip.clip.GetFrames().front()
                                                                   : Animation::KeyFrame{}}
    , timeForChange_{context.transitionTime}
    , currentTime_{0.f}
    , startTime_{0.f}
{
}
bool AnimationTransition::update(float deltaTime)
{
    interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, endChangeAnimKeyFrame_, currentTime_);
    calculateTime(deltaTime);
    return true;
}
const std::string& AnimationTransition::getAnimationClipName() const
{
    return info_.clip.name;
}

void AnimationTransition::handle(const ChangeAnimationEvent&)
{
}

void AnimationTransition::handle(const StopAnimationEvent&)
{
}
void AnimationTransition::calculateTime(float deltaTime)
{
    currentTime_ += (1.f / timeForChange_) * deltaTime;
    if (currentTime_ > 1.f)
    {
        context_.machine.transitionTo(std::make_unique<PlayAnimation>(context_, info_, startTime_));
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
