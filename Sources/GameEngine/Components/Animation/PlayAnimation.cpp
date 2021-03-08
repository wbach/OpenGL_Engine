#include "PlayAnimation.h"

#include "AnimationTransition.h"
#include "AnimationTransitionGrouped.h"
#include "EmptyState.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayAnimation::PlayAnimation(Context& context, const AnimationClipInfo& info, float startTime)
    : context_{context}
    , time_{startTime}
    , clipInfo_{info}
{
}
bool PlayAnimation::update(float deltaTime)
{
    calculateCurrentAnimationPose(context_.currentPose, clipInfo_.clip, time_);
    increaseAnimationTime(deltaTime);
    return true;
}
const std::string& PlayAnimation::getAnimationClipName() const
{
    return clipInfo_.clip.name;
}

void PlayAnimation::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        std::vector<uint32> jointIds;
        for (auto& [name, group] : context_.jointGroups)
        {
            if (name != event.jointGroupName)
            {
                jointIds.insert(jointIds.end(), group.begin(), group.end());
            }
        }

        std::vector<CurrentGroupsPlayingInfo> v{{clipInfo_, time_, jointIds}};
        context_.machine.transitionTo(std::make_unique<AnimationTransitionGrouped>(context_, v, event));
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, event.startTime, event.info));
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
            for (const auto& callback : clipInfo_.endCallbacks_)
            {
                callback();
            }

            context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
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
