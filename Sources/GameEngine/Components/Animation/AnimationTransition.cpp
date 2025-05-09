#include "AnimationTransition.h"

#include <Logger/Log.h>

#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "StateMachine.h"
#include "TransitionGroupsPlaying.h"

namespace GameEngine
{
namespace Components
{
AnimationTransition::AnimationTransition(Context& context, const AnimationClipInfo& infoClip, float /*startTime*/,
                                         OnTransitionEnd onTransitionEnd)
    : AnimationStateBase{context}
    , data{{.time = 0.f, .clipInfo = infoClip, .jointGroup = nullptr},
           0.f,
           context.transitionTime,
           onTransitionEnd,
           convert(context.currentPose),  // to do , calculate pose for startTime
           not infoClip.clip.GetFrames().empty() ? infoClip.clip.GetFrames().front() : Animation::KeyFrame{}}
{
}
bool AnimationTransition::update(float deltaTime)
{
    interpolatePoses(context_.currentPose, data.startChaneAnimKeyFrame, data.endChangeAnimKeyFrame, data.time);
    calculateTime(deltaTime);
    return true;
}
void AnimationTransition::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        std::vector<TransitionGroupsPlaying> currentAnimtionTransitionInfo{{data.clipInfo, data.time, {}, data.onTransitionEnd}};
        auto& jointGroupNames = currentAnimtionTransitionInfo.front();

        bool groupFound{false};
        for (auto& [name, group] : context_.jointGroups)
        {
            if (name != event.jointGroupName)
            {
                jointGroupNames.jointGroupNames.push_back(name);
            }
            else
            {
                groupFound = true;
            }
        }

        if (groupFound)
        {
            context_.machine.transitionTo<AnimationTransitionToMixed>(context_, currentAnimtionTransitionInfo, event);
        }
        else
        {
            WARNING_LOG("Try change animation for non exisitng group: " + *event.jointGroupName);
        }
    }
    else
    {
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime, event.onTransitionEnd);
    }
}

void AnimationTransition::handle(const StopAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }
    else
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }
}

std::vector<std::string> AnimationTransition::getCurrentAnimation() const
{
    return {data.clipInfo.clip.getName()};
}

void AnimationTransition::calculateTime(float deltaTime)
{
    data.time += (deltaTime / data.timeForChange);

    if (data.time > 1.f)
    {
        if (data.onTransitionEnd)
        {
            data.onTransitionEnd();
        }

        context_.machine.transitionTo<PlayAnimation>(context_, data.clipInfo, data.startupTime);
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
