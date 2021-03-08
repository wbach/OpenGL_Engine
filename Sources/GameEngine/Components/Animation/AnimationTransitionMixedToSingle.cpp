#include "AnimationTransitionMixedToSingle.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "PlayMixedAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string clipName{"AnimationTransitionMixedToSingle"};
}

AnimationTransitionMixedToSingle::AnimationTransitionMixedToSingle(Context &context,
                                                                   const CurrentGroupsPlayingInfo &info)
    : context_{context}
    , currentClipProgres_{info.currentTime}
    , currentClipInfo_{info.info}
{
    DEBUG_LOG("");

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
        else
        {
            transitionGroups_.push_back({iter->second});
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
        interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, nextKeyFrame, transitionProgress_,
                         transitionGroup.jointGroups);
    }

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

const std::string &AnimationTransitionMixedToSingle::getAnimationClipName() const
{
    return clipName;
}

void AnimationTransitionMixedToSingle::handle(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        DEBUG_LOG("not implmented.");
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, event.info, event.startTime));
    }
}

void AnimationTransitionMixedToSingle::handle(const StopAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        DEBUG_LOG("not implemented");
        // context_.machine.transitionTo(std::make_unique<PlayAnimation>(context_));
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
    }
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
        AnimationClipInfoPerGroup infoPerGroup;

        for (const auto &group : currentGroups_)
        {
        }
        context_.machine.transitionTo(std::make_unique<PlayMixedAnimation>(context_, infoPerGroup));
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
