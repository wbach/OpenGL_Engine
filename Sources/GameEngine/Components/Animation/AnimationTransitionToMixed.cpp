#include "AnimationTransitionToMixed.h"

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
const std::string clipName{"AnimationTransitionToMixed"};
}

AnimationTransitionToMixed::AnimationTransitionToMixed(
    Context &context, const std::vector<CurrentGroupsPlayingInfo> &currentGroupsPlayingInfos,
    const ChangeAnimationEvent &event)
    : context_{context}
    , secondaryClipStartupTime_{event.startTime}
    , secondaryClipInfo_{event.info}
    , secondaryAnimJointGroup_{nullptr}
    , startChaneAnimKeyFrame_{convert(context_.currentPose)}
    , timeForChange_{context_.transitionTime}
    , transitionProgress_{0.f}
{
    DEBUG_LOG("");
    for (auto &info : currentGroupsPlayingInfos)
    {
        for (auto &groupName : info.jointGroupNames)
        {
            auto iter = context.jointGroups.find(groupName);

            if (iter != context_.jointGroups.end())
            {
                currentGroups_.push_back({info.info, info.currentTime, iter->second, groupName});
            }
        }
    }
    if (event.jointGroupName)
    {
        secondaryGroupName = *event.jointGroupName;
        auto iter = context_.jointGroups.find(*event.jointGroupName);
        if (iter != context_.jointGroups.end())
        {
            secondaryAnimJointGroup_ = &iter->second;
        }
    }
}

bool AnimationTransitionToMixed::update(float deltaTime)
{
    for (const auto &group : currentGroups_)
    {
        if (not group.jointGroup_.empty())
        {
            calculateCurrentAnimationPose(context_.currentPose, group.clipInfo_.clip, group.progres_,
                                          group.jointGroup_);
        }
    }

    if (secondaryAnimJointGroup_)
    {
        const auto &endChangeAnimKeyFrame = secondaryClipInfo_.clip.GetFrames().front();
        interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, endChangeAnimKeyFrame, transitionProgress_,
                         *secondaryAnimJointGroup_);
    }

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

const std::string &AnimationTransitionToMixed::getAnimationClipName() const
{
    return clipName;
}

void AnimationTransitionToMixed::handle(const ChangeAnimationEvent &event)
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

void AnimationTransitionToMixed::handle(const StopAnimationEvent &event)
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

void AnimationTransitionToMixed::increaseAnimationTime(float deltaTime)
{
    for (auto &group : currentGroups_)
    {
        auto length    = group.clipInfo_.clip.GetLength();
        auto &progress = group.progres_;

        progress += deltaTime * group.clipInfo_.playSpeed; //* direction_;

        if (progress > length)
        {
            progress = fmodf(progress, length);
        }
        if (progress < 0)
        {
            progress = group.clipInfo_.clip.GetLength() + progress;
        }
    }
}

void AnimationTransitionToMixed::increaseTransitionTime(float deltaTime)
{
    transitionProgress_ += (1.f / timeForChange_) * deltaTime;

    if (transitionProgress_ > 1.f)
    {
        AnimationClipInfoPerGroup infoPerGroup;
        for (const auto &group : currentGroups_)
        {
            infoPerGroup.insert({group.name, {group.clipInfo_, group.progres_}});
        }

        infoPerGroup.insert({secondaryGroupName, {secondaryClipInfo_, 0.f}});
        context_.machine.transitionTo(std::make_unique<PlayMixedAnimation>(context_, infoPerGroup));
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
