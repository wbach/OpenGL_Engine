#include "AnimationTransitionGrouped.h"

#include <Logger/Log.h>
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "PlayMixedAnimation.h"
#include "AnimationTransition.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string clipName{"AnimationTransitionGrouped"};
}

AnimationTransitionGrouped::AnimationTransitionGrouped(
    Context &context, const std::vector<CurrentGroupsPlayingInfo> &currentGroupsPlayingInfos,
    const ChangeAnimationEvent &event)
    : context_{context}
    , secondaryClipStartupTime_{event.startTime}
    , secondaryClipInfo_{event.info}
{
    for (auto & info : currentGroupsPlayingInfos)
    {
        currentGroups_.push_back({info.info, info.currentTime, info.jointsGroup});
    }

    startChaneAnimKeyFrame_ = convert(context_.currentPose);
}

bool AnimationTransitionGrouped::update(float deltaTime)
{
    for (const auto &group : currentGroups_)
    {
        if (not group.currentAnimJointGroup_.empty())
        {
            calculateCurrentAnimationPose(context_.currentPose, group.currentClipInfo_.clip, group.currentClipProgres_,
                                          group.currentAnimJointGroup_);
        }
    }

    const auto& endChangeAnimKeyFrame = secondaryClipInfo_.clip.GetFrames().front();
        interpolatePoses(context_.currentPose, startChaneAnimKeyFrame_, endChangeAnimKeyFrame, transitionProgress_, secondaryAnimJointGroup_);

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

const std::string &AnimationTransitionGrouped::getAnimationClipName() const
{
    return clipName;
}

void AnimationTransitionGrouped::handle(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        DEBUG_LOG("not implmented.");
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, event.startTime, event.info));
    }
}

void AnimationTransitionGrouped::handle(const StopAnimationEvent &event)
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

void AnimationTransitionGrouped::increaseAnimationTime(float deltaTime)
{
    for (auto &group : currentGroups_)
    {
        group.currentClipProgres_ += deltaTime * group.currentClipInfo_.playSpeed;  //* direction_;

        if (group.currentClipProgres_ > group.currentClipInfo_.clip.GetLength())
        {
            group.currentClipProgres_ = fmodf(group.currentClipProgres_, group.currentClipInfo_.clip.GetLength());
        }
        if (group.currentClipProgres_ < 0)
        {
            group.currentClipProgres_ = group.currentClipInfo_.clip.GetLength() + group.currentClipProgres_;
        }
    }
}

void AnimationTransitionGrouped::increaseTransitionTime(float deltaTime)
{
    transitionProgress_ += (1.f / timeForChange_) * deltaTime;

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
