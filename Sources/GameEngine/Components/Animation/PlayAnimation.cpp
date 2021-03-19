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
{
}
bool PlayAnimation::update(float deltaTime)
{
    calculateCurrentAnimationPose(context_.currentPose, clipInfo_.clip, time_);
    increaseAnimationTime(deltaTime);
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
        context_.machine.transitionTo(std::make_unique<AnimationTransitionToMixed>(context_, v, event));
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, event.info, event.startTime));
    }
}

void PlayAnimation::handle(const StopAnimationEvent&)
{
    context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
}

void PlayAnimation::increaseAnimationTime(float deltaTime)
{
    time_ += deltaTime * clipInfo_.playSpeed * direction_;

    if (time_ > clipInfo_.clip.GetLength())
    {
        if (clipInfo_.clip.playType == Animation::AnimationClip::PlayType::once)
        {
            context_.machine.transitionTo(std::make_unique<EmptyState>(context_));

            for (const auto& [_, callback] : clipInfo_.endCallbacks_)
            {
                callback();
            }
            return;
        }

        time_ = fmodf(time_, clipInfo_.clip.GetLength());
    }
    if (time_ < 0)
    {
        time_ = clipInfo_.clip.GetLength() + time_;
    }
}
}  // namespace Components
}  // namespace GameEngine
