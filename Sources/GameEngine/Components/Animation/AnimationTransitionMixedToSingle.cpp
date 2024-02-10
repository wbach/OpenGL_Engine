#include "AnimationTransitionMixedToSingle.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "PlayMixedAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
AnimationTransitionMixedToSingle::AnimationTransitionMixedToSingle(Context &context,
                                                                   const CurrentGroupsPlayingInfo &info)
    : context_{context}
    , currentClipProgres_{info.currentTime}
    , currentClipInfo_{info.info}
    , transitionProgress_{0.f}
{
    for (auto &groupName : info.jointGroupNames)
    {
        auto iter = context.jointGroups.find(groupName);

        if (iter != context_.jointGroups.end())
        {
            currentGroups_.push_back({
                groupName,
                iter->second,
            });
        }
    }

    for (const auto &[name, group] : context_.jointGroups)
    {
        auto iter = std::find(info.jointGroupNames.begin(), info.jointGroupNames.end(), name);
        if (iter == info.jointGroupNames.end())
        {
            transitionGroups_.push_back({group});
        }
    }

    startChaneAnimKeyFrame_ = convert(context_.currentPose);
}

bool AnimationTransitionMixedToSingle::update(float deltaTime)
{
    for (const auto &group : currentGroups_)
    {
        if (not group.currentAnimJointGroup_.empty())
        {
            calculateCurrentAnimationPose(context_.currentPose, currentClipInfo_.clip, currentClipProgres_,
                                          group.currentAnimJointGroup_);
        }
    }

    for (const auto &transitionGroup : transitionGroups_)
    {
        auto [_, nextKeyFrame] = getPreviousAndNextFrames(currentClipInfo_.clip, currentClipProgres_);
        interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, *nextKeyFrame, transitionProgress_,
                         transitionGroup.jointGroups);
    }

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

void AnimationTransitionMixedToSingle::handle(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        std::vector<CurrentGroupsPlayingInfo> v{{currentClipInfo_, currentClipProgres_, {}}};

        for (auto &[name, group] : context_.jointGroups)
        {
            if (name != event.jointGroupName)
            {
                v.front().jointGroupNames.push_back(name);
            }
        }
        context_.machine.transitionTo<AnimationTransitionToMixed>(context_, v, event, event.onTransitionEnd);
    }
    else
    {
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime,
                                                           event.onTransitionEnd);
    }
}

void AnimationTransitionMixedToSingle::handle(const StopAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        DEBUG_LOG("not implemented");
        context_.machine.transitionTo<EmptyState>(context_);
        // context_.machine.transitionTo(std::make_unique<PlayAnimation>(context_));
    }
    else
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }
}

std::vector<std::string> AnimationTransitionMixedToSingle::getCurrentAnimation() const
{
    return {currentClipInfo_.clip.name};
}

bool AnimationTransitionMixedToSingle::isAnimationPlaying(const std::string &name) const
{
    return currentClipInfo_.clip.name == name;
}

void AnimationTransitionMixedToSingle::increaseAnimationTime(float deltaTime)
{
    currentClipProgres_ += deltaTime * currentClipInfo_.playSpeed;  //* direction_;

    if (currentClipProgres_ > currentClipInfo_.clip.GetLength())
    {
        currentClipProgres_ = fmodf(currentClipProgres_, currentClipInfo_.clip.GetLength());
    }
    if (currentClipProgres_ < 0)
    {
        currentClipProgres_ = currentClipInfo_.clip.GetLength() + currentClipProgres_;
    }
}

void AnimationTransitionMixedToSingle::increaseTransitionTime(float deltaTime)
{
    transitionProgress_ += (1.f / context_.transitionTime) * deltaTime;

    if (transitionProgress_ > 1.f)
    {
        context_.machine.transitionTo<PlayAnimation>(context_, currentClipInfo_, currentClipProgres_);
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
