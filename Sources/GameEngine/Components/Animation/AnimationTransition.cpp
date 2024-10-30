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
    : context{context}
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
    interpolatePoses(context.currentPose, data.startChaneAnimKeyFrame, data.endChangeAnimKeyFrame, data.time);
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
        for (auto& [name, group] : context.jointGroups)
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
            context.machine.transitionTo<AnimationTransitionToMixed>(context, currentAnimtionTransitionInfo, event);
        }
        else
        {
            WARNING_LOG("Try change animation for non exisitng group: " + *event.jointGroupName);
        }
    }
    else
    {
        context.machine.transitionTo<AnimationTransition>(context, event.info, event.startTime, event.onTransitionEnd);
    }
}

void AnimationTransition::handle(const StopAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        context.machine.transitionTo<EmptyState>(context);
    }
    else
    {
        context.machine.transitionTo<EmptyState>(context);
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

        context.machine.transitionTo<PlayAnimation>(context, data.clipInfo, data.startupTime);
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
