#include "PlayMixedAnimation.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "AnimationTransitionMixedToSingle.h"
#include "AnimationTransitionToMixed.h"
#include "EmptyState.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "PlayAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{

struct Core
{
    float time;
    float direction;
    const AnimationClipInfo& clipInfo;
    const std::vector<uint32>& jointGroup;
};

struct PlayGroup
{
    Core core;
    CurrentFrames frames{nullptr, nullptr};
    float previousFrameTimeStamp{-1.0f};
};

PlayMixedAnimation::PlayMixedAnimation(Context& context, const AnimationClipInfoPerGroup& animationPlayingInfoPerGroup)
    : context_{context}
{
    for (auto& [name, clipInfoPair] : animationPlayingInfoPerGroup)
    {
        auto iter = context.jointGroups.find(name);
        if (iter != context.jointGroups.end())
        {
            const auto& [info, startTime] = clipInfoPair;

            const auto& jointGroup = iter->second;
            float direction{info.playDirection == PlayDirection::forward ? 1.f : -1.f};
            groups_.insert(
                {name,
                 PlayGroup{.core = {.time = startTime, .direction = direction, .clipInfo = info, .jointGroup = jointGroup}}});
        }
    }
}
bool PlayMixedAnimation::update(float deltaTime)
{
    for (auto& [name, group] : groups_)
    {
        calculateCurrentAnimationPose(context_.currentPose, group.core.clipInfo.clip, group.core.time, group.core.jointGroup);
        group.frames = context_.currentPose.frames;
    }

    increaseAnimationTime(deltaTime);
    return true;
}
void PlayMixedAnimation::handle(const ChangeAnimationEvent& event)
{
    if (event.jointGroupName)
    {
        std::vector<CurrentGroupsPlayingInfo> infos{};
        for (auto& [name, group] : groups_)
        {
            if (name != event.jointGroupName)
            {
                infos.push_back(CurrentGroupsPlayingInfo{group.core.clipInfo, group.core.time, {name}});
            }
        }

        context_.machine.transitionTo<AnimationTransitionToMixed>(context_, infos, event);
    }
    else
    {
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime);
    }
}

void PlayMixedAnimation::handle(const StopAnimationEvent& event)
{
    if (event.jointGroupName and groups_.size() > 1)
    {
        if (groups_.size() > 2)
        {
            auto iter = groups_.find(*event.jointGroupName);
            if (iter != groups_.end())
            {
                groups_.erase(iter);
            }
        }
        else
        {
            for (auto& [name, group] : groups_)
            {
                if (name != *event.jointGroupName)
                {
                    CurrentGroupsPlayingInfo info{group.core.clipInfo, group.core.time, {name}};
                    context_.machine.transitionTo<AnimationTransitionMixedToSingle>(context_, info);
                    return;
                }
            }
        }
    }
    else
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }
}

std::vector<std::string> PlayMixedAnimation::getCurrentAnimation() const
{
    std::vector<std::string> r;
    for (auto& [_, group] : groups_)
        r.push_back(group.core.clipInfo.clip.getName());
    return r;
}
bool PlayMixedAnimation::isAnimationPlaying(const std::string& name) const
{
    auto iter = std::find_if(groups_.begin(), groups_.end(),
                             [&name](const auto& pair) { return (pair.second.core.clipInfo.clip.getName() == name); });
    return iter != groups_.end();
}
void PlayMixedAnimation::increaseAnimationTime(float deltaTime)
{
    std::vector<std::string> groupsToRemove_;

    for (auto& [name, group] : groups_)
    {
        auto& core = group.core;
        core.time += deltaTime * core.clipInfo.playSpeed * core.direction;

        notifyFrameSubsribers(core.clipInfo, group.frames.first, core.time, group.previousFrameTimeStamp);

        if (core.time > group.core.clipInfo.clip.GetLength())
        {
            if (group.core.clipInfo.playType == AnimationClipInfo::PlayType::once)
            {
                groupsToRemove_.push_back(name);
                continue;
            }
            core.time = fmodf(core.time, group.core.clipInfo.clip.GetLength());
        }
        if (core.time < 0)
        {
            core.time = group.core.clipInfo.clip.GetLength() + core.time;
        }
    }

    if (groupsToRemove_.size() == groups_.size())
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }
    else if (groupsToRemove_.size() == groups_.size() - 1)
    {
        for (auto& toRemoveName : groupsToRemove_)
        {
            for (auto& [name, group] : groups_)
            {
                if (name != toRemoveName)
                {
                    CurrentGroupsPlayingInfo info{group.core.clipInfo, group.core.time, {name}};
                    DEBUG_LOG("increaseAnimationTime AnimationTransitionMixedToSingle");
                    context_.machine.transitionTo<AnimationTransitionMixedToSingle>(context_, info);
                    return;
                }
            }
        }
    }
    else
    {
        for (auto& name : groupsToRemove_)
        {
            groups_.erase(name);
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
