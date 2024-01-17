#include "AnimationTransition.h"

#include <Logger/Log.h>

#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
AnimationTransition::AnimationTransition(Context& context, const AnimationClipInfo& infoClip, float /*startTime*/,
                                         OnTransitionEnd onTransitionEnd)
    : context_{context}
    , info_{infoClip}
    , startChaneAnimKeyFrame_{convert(context_.currentPose)}  // to do , calculate pose for startTime
    , endChangeAnimKeyFrame_{not infoClip.clip.GetFrames().empty() ? infoClip.clip.GetFrames().front()
                                                                   : Animation::KeyFrame{}}
    , timeForChange_{context.transitionTime}
    , currentTime_{0.f}
    , startTime_{0.f}
    , onTransitionEnd_{onTransitionEnd}
{
}
bool AnimationTransition::update(float deltaTime)
{
    interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, endChangeAnimKeyFrame_, currentTime_);
    calculateTime(deltaTime);
    return true;
}
void AnimationTransition::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        std::vector<CurrentGroupsPlayingInfo> v{{info_, 0.f, {}}};

        for (auto& [name, group] : context_.jointGroups)
        {
            if (name != event.jointGroupName)
            {
                v.front().jointGroupNames.push_back(name);
            }
        }
        context_.machine.transitionTo(std::make_unique<AnimationTransitionToMixed>(context_, v, event));
    }
    else
    {
        context_.machine.transitionTo(
            std::make_unique<AnimationTransition>(context_, event.info, event.startTime, event.onTransitionEnd));
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

std::vector<std::string> AnimationTransition::getCurrentAnimation() const
{
    return {info_.clip.name};
}
void AnimationTransition::calculateTime(float deltaTime)
{
    currentTime_ += (deltaTime / timeForChange_);

    if (currentTime_ > 1.f)
    {
        if (onTransitionEnd_)
        {
            onTransitionEnd_();
        }
        context_.machine.transitionTo(std::make_unique<PlayAnimation>(context_, info_, startTime_));
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
