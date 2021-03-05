#include "PlayAnimation.h"

#include "AnimationTransition.h"
#include "AnimationTransitionGrouped.h"
#include "StateMachine.h"
#include "EmptyState.h"

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
        context_.machine.transitionTo(std::make_unique<AnimationTransitionGrouped>(clipInfo_, time_, event));
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(event.startTime, event.info));
    }
}

void PlayAnimation::handle(const StopAnimationEvent&)
{
    context_.machine.transitionTo(std::make_unique<EmptyState>());
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

            context_.machine.transitionTo(std::make_unique<EmptyState>());
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
