#include "AnimationTransition.h"

#include <Logger/Log.h>

#include "EmptyState.h"
#include "PlayAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
AnimationTransition::AnimationTransition(Context& context, const AnimationClipInfo& infoClip, float /*startTime*/)
    : context_{context}
    , info_{infoClip}
    , startChaneAnimKeyFrame_{convert(context_.currentPose)}  // to do , calculate pose for startTime
    , endChangeAnimKeyFrame_{not infoClip.clip.GetFrames().empty() ? infoClip.clip.GetFrames().front()
                                                                   : Animation::KeyFrame{}}
    , timeForChange_{context.transitionTime}
    , currentTime_{0.f}
    , startTime_{0.f}
{
    DEBUG_LOG("");
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

void AnimationTransition::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        // context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, info_, startTime_));
        DEBUG_LOG("Not implmented");
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, event.info, event.startTime));
    }
}

void AnimationTransition::handle(const StopAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        DEBUG_LOG("not implemented");
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
    }
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
