#include "AnimationTransitionMixedToSingle.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "StateMachine.h"
#include "Context.h"
#include "AnimationClipInfo.h"
#include "CurrentGroupsPlayingInfo.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::vector<uint32> *findJointGroup(Context &context, const CurrentGroupsPlayingInfo &info)
{
    if (info.jointGroupNames.size() != 1)
    {
        LOG_WARN << "Wrong group size! Note: multiple joint groups to single anim not implemented. Size="
                 << info.jointGroupNames.size();
        return nullptr;
    }

    const auto &groupName = info.jointGroupNames.front();
    auto iter             = context.jointGroups.find(groupName);

    if (iter != context.jointGroups.end())
    {
        return &iter->second;
    }

    LOG_WARN << "Joint group not found! Group name :" << groupName;
    return nullptr;
}
}  // namespace
AnimationTransitionMixedToSingle::AnimationTransitionMixedToSingle(Context &context, const CurrentGroupsPlayingInfo &info)
    : AnimationStateBase{context}
    , currentAnim{{.time = info.currentTime, .clipInfo = info.info, .jointGroup = findJointGroup(context, info)}}
    , transitionProgress_{0.f}
{
    for (const auto &[name, group] : context_.jointGroups)
    {
        auto iter = std::find(info.jointGroupNames.begin(), info.jointGroupNames.end(), name);
        if (iter == info.jointGroupNames.end())
        {
            transitionGroups_.push_back({group});
        }
    }
    startChangeAnimKeyFrame_ = convert(context_.currentPose);
}

bool AnimationTransitionMixedToSingle::update(float deltaTime)
{
    if (currentAnim.jointGroup and not currentAnim.jointGroup->empty())
    {
        calculateCurrentAnimationPose(context_.currentPose, currentAnim.clipInfo.clip, currentAnim.time, *currentAnim.jointGroup);
    }

    for (const auto &transitionGroup : transitionGroups_)
    {
        auto [_, nextKeyFrame] = getPreviousAndNextFrames(currentAnim.clipInfo.clip, currentAnim.time);
        interpolatePoses(context_.currentPose, startChangeAnimKeyFrame_, *nextKeyFrame, transitionProgress_,
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
        std::vector<CurrentGroupsPlayingInfo> v{{currentAnim.clipInfo, currentAnim.time, {}}};

        for (auto &[name, group] : context_.jointGroups)
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
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime, event.onTransitionEnd);
    }
}

void AnimationTransitionMixedToSingle::handle(const StopAnimationEvent &event)
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

std::vector<std::string> AnimationTransitionMixedToSingle::getCurrentAnimation() const
{
    return {currentAnim.clipInfo.clip.getName()};
}

bool AnimationTransitionMixedToSingle::isAnimationPlaying(const std::string &name) const
{
    return currentAnim.clipInfo.clip.getName() == name;
}

void AnimationTransitionMixedToSingle::increaseAnimationTime(float deltaTime)
{
    currentAnim.time += deltaTime * currentAnim.clipInfo.playSpeed;  //* direction_;

    if (currentAnim.time > currentAnim.clipInfo.clip.GetLength())
    {
        currentAnim.time = fmodf(currentAnim.time, currentAnim.clipInfo.clip.GetLength());
    }
    if (currentAnim.time < 0)
    {
        currentAnim.time = currentAnim.clipInfo.clip.GetLength() + currentAnim.time;
    }
}

void AnimationTransitionMixedToSingle::increaseTransitionTime(float deltaTime)
{
    transitionProgress_ += (1.f / context_.transitionTime) * deltaTime;

    if (transitionProgress_ > 1.f)
    {
        context_.machine.transitionTo<PlayAnimation>(context_, currentAnim.clipInfo, currentAnim.time);
        return;
    }
}
}  // namespace Components
}  // namespace GameEngine
