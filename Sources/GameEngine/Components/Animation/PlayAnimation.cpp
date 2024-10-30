#include "PlayAnimation.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayAnimation::PlayAnimation(Context& context, const AnimationClipInfo& info, float startTime)
    : context_{context}
    , time_{startTime}
    , direction_{info.playDirection == PlayDirection::forward ? 1.f : -1.f}
    , clipInfo_{info}
    , previousFrameTimeStamp{-1.0f}
{
}
bool PlayAnimation::update(float deltaTime)
{
    calculateCurrentAnimationPose(context_.currentPose, clipInfo_.clip, time_);

    if (not increaseAnimationTime(time_, previousFrameTimeStamp, clipInfo_,
                                                      context_.currentPose.frames.first, deltaTime))
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }

    return true;
}
void PlayAnimation::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        std::vector<CurrentGroupsPlayingInfo> v{{clipInfo_, time_, {}}};

        for (auto& [name, group] : context_.jointGroups)
        {
            if (name != event.jointGroupName)
            {
                v.front().jointGroupNames.push_back(name);
            }
        }
        context_.machine.transitionTo<AnimationTransitionToMixed>(context_, v, event);
    }
    else
    {
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime,
                                                           event.onTransitionEnd);
    }
}

void PlayAnimation::handle(const StopAnimationEvent& e)
{
    context_.machine.transitionTo<EmptyState>(context_);
}

std::vector<std::string> PlayAnimation::getCurrentAnimation() const
{
    return {clipInfo_.clip.getName()};
}

bool PlayAnimation::isAnimationPlaying(const std::string& name) const
{
    return clipInfo_.clip.getName() == name;
}
}  // namespace Components
}  // namespace GameEngine
