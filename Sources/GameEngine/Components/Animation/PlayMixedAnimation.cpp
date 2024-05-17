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
PlayMixedAnimation::PlayMixedAnimation(Context& context, const AnimationClipInfoPerGroup& animationPlayingInfoPerGroup)
    : context_{context}
{
    for (auto& [name, pair] : animationPlayingInfoPerGroup)
    {
        auto iter = context.jointGroups.find(name);
        if (iter != context.jointGroups.end())
        {
            auto& info     = pair.first;
            auto startTime = pair.second;

            const auto& jointGroups = iter->second;
            float direction{info.playDirection == PlayDirection::forward ? 1.f : -1.f};
            groups_.insert({name, {info, startTime, direction, jointGroups}});
        }
    }
}
bool PlayMixedAnimation::update(float deltaTime)
{
    for (auto& [name, group] : groups_)
    {
        calculateCurrentAnimationPose(context_.currentPose, group.clipInfo.clip, group.time, group.jointIds);
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
                infos.push_back(CurrentGroupsPlayingInfo{group.clipInfo, group.time, {name}});
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
                    context_.machine.transitionTo<AnimationTransition>(context_, group.clipInfo, group.time);
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
        r.push_back(group.clipInfo.clip.name);
    return r;
}
bool PlayMixedAnimation::isAnimationPlaying(const std::string& name) const
{
    auto iter = std::find_if(groups_.begin(), groups_.end(),
                             [&name](const auto& pair) { return (pair.second.clipInfo.clip.name == name); });
    return iter != groups_.end();
}
void PlayMixedAnimation::increaseAnimationTime(float deltaTime)
{
    std::vector<std::string> groupsToRemove_;

    for (auto& [name, group] : groups_)
    {
        group.time += deltaTime * group.clipInfo.playSpeed * group.direction;

        notifyFrameSubsribers(group.clipInfo, group.frames.first, group.time, group.previousFrameTimeStamp);

        if (group.time > group.clipInfo.clip.GetLength())
        {
            if (group.clipInfo.clip.playType == Animation::AnimationClip::PlayType::once)
            {
                groupsToRemove_.push_back(name);
                continue;
            }
            group.time = fmodf(group.time, group.clipInfo.clip.GetLength());
        }
        if (group.time < 0)
        {
            group.time = group.clipInfo.clip.GetLength() + group.time;
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
                    CurrentGroupsPlayingInfo info{group.clipInfo, group.time, {name}};
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
