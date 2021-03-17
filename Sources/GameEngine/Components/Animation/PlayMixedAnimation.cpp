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
    , animationName_{"mixed"}
{
    DEBUG_LOG("");

    for (auto& [name, pair] : animationPlayingInfoPerGroup)
    {
        auto iter = context.jointGroups.find(name);

        if (iter != context.jointGroups.end())
        {
            auto& info     = pair.first;
            auto startTime = pair.second;
            DEBUG_LOG(info.clip.name);

            const auto& jointGroups = iter->second;
            float direction{info.playDirection == PlayDirection::forward ? 1.f : -1.f};
            groups_.insert({name, {info, startTime, direction, jointGroups}});
        }
    }
}
bool PlayMixedAnimation::update(float deltaTime)
{
    for (auto& [_, group] : groups_)
    {
        calculateCurrentAnimationPose(context_.currentPose, group.clipInfo.clip, group.time, group.jointIds);
    }

    increaseAnimationTime(deltaTime);
    return true;
}
const std::string& PlayMixedAnimation::getAnimationClipName() const
{
    return animationName_;
}

void PlayMixedAnimation::handle(const ChangeAnimationEvent& event)
{
    DEBUG_LOG("PlayMixedAnimation " + event.info.clip.name);
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
        DEBUG_LOG("AnimationTransitionToMixed " + *event.jointGroupName);
        context_.machine.transitionTo(std::make_unique<AnimationTransitionToMixed>(context_, infos, event));
    }
    else
    {
        DEBUG_LOG("AnimationTransition");
        context_.machine.transitionTo(std::make_unique<AnimationTransition>(context_, event.info, event.startTime));
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
                    context_.machine.transitionTo(
                        std::make_unique<AnimationTransition>(context_, group.clipInfo, group.time));
                    return;
                }
            }
        }
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
    }
}
void PlayMixedAnimation::increaseAnimationTime(float deltaTime)
{
    std::vector<std::string> groupsToRemove_;

    for (auto& [name, group] : groups_)
    {
        group.time += deltaTime * group.clipInfo.playSpeed * group.direction;

        if (group.time > group.clipInfo.clip.GetLength())
        {
            if (group.clipInfo.clip.playType == Animation::AnimationClip::PlayType::once)
            {
                for (const auto& [_, callback] : group.clipInfo.endCallbacks_)
                {
                    callback();
                }

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

    if (context_.machine.transitionState_)
        return;

    if (groupsToRemove_.size() == groups_.size())
    {
        DEBUG_LOG("EmptyState");
        context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
    }
    else if (groupsToRemove_.size() == groups_.size() - 1)
    {
        for (auto& toRemoveName : groupsToRemove_)
        {
            for (auto& [name, group] : groups_)
            {
                if (name != toRemoveName)
                {
                    DEBUG_LOG("AnimationTransitionMixedToSingle");
                    CurrentGroupsPlayingInfo info{group.clipInfo, group.time, {name}};
                    context_.machine.transitionTo(std::make_unique<AnimationTransitionMixedToSingle>(context_, info));
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
