#include "AnimationTransitionToMixed.h"

#include <Logger/Log.h>
#include <Utils/Container.h>

#include <algorithm>

#include "AnimationTransition.h"
#include "AnimationTransitionMixedToSingle.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "PlayMixedAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
AnimationTransitionToMixed::AnimationTransitionToMixed(Context &context, const CurrentGroupsPlayingInfos &current,
                                                       const ChangeAnimationEvent &event)
    : AnimationTransitionToMixed(context, current, {}, event)
{
}

AnimationTransitionToMixed::AnimationTransitionToMixed(Context &context, const TransitionGroupsPlayings &transitionGroupsPlaying,
                                                       const ChangeAnimationEvent &event)
    : AnimationTransitionToMixed(context, {}, transitionGroupsPlaying, event)
{
}

AnimationTransitionToMixed::AnimationTransitionToMixed(Context &context,
                                                       const CurrentGroupsPlayingInfos &currentGroupsPlayingInfos,
                                                       const TransitionGroupsPlayings &transitionGroupsPlaying,
                                                       const ChangeAnimationEvent &event)
    : context{context}
{
    for (auto &info : currentGroupsPlayingInfos)
    {
        for (auto &groupName : info.jointGroupNames)
        {
            auto iter = context.jointGroups.find(groupName);

            if (iter != context.jointGroups.end())
            {
                const auto &jointGroup = iter->second;
                currentGroups.insert(
                    {groupName, PlayGroupMixed{{{.time = info.currentTime, .clipInfo = info.info, .jointGroup = &jointGroup},
                                                info.info.playDirection == PlayDirection::forward ? 1.f : -1.f}}});
            }
        }
    }

    for (auto &info : transitionGroupsPlaying)
    {
        for (auto &groupName : info.jointGroupNames)
        {
            auto iter = context.jointGroups.find(groupName);

            if (iter != context.jointGroups.end())
            {
                const auto &jointGroup = iter->second;
                transtionGroups.insert(
                    {groupName, TransitionGroup{{.time = info.currentTime, .clipInfo = info.info, .jointGroup = &jointGroup},
                                                0.f,
                                                context.transitionTime,
                                                info.onTransitionEnd,
                                                convert(context.currentPose)}});
            }
        }
    }

    addTransitionBasedOnEvent(event);
}

bool AnimationTransitionToMixed::update(float deltaTime)
{
    for (auto &[name, group] : currentGroups)
    {
        if (group.jointGroup and not group.jointGroup->empty())
        {
            calculateCurrentAnimationPose(context.currentPose, group.clipInfo.clip, group.time, *group.jointGroup);
            group.frames = context.currentPose.frames;
        }
    }

    for (const auto &[name, group] : transtionGroups)
    {
        if (group.jointGroup)
        {
            const auto &endChangeAnimKeyFrame = group.clipInfo.clip.GetFrames().front();
            interpolatePoses(context.currentPose, group.startChaneAnimKeyFrame, endChangeAnimKeyFrame, group.time,
                             *group.jointGroup);
        }
    }

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

void AnimationTransitionToMixed::handle(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = transtionGroups.find(*event.jointGroupName);

        if (iter != transtionGroups.end())
        {
            TransitionGroup newGroup{{.time = 0.f, .clipInfo = event.info, .jointGroup = iter->second.jointGroup},
                                     event.startTime,
                                     context.transitionTime,
                                     event.onTransitionEnd,
                                     convert(context.currentPose)};

            transtionGroups.erase(iter);
            transtionGroups.insert({*event.jointGroupName, newGroup});
        }
        else
        {
            auto currentGroupsIter = currentGroups.find(*event.jointGroupName);

            if (currentGroupsIter != currentGroups.end())
            {
                TransitionGroup newGroup{
                    {.time = 0.f, .clipInfo = event.info, .jointGroup = currentGroupsIter->second.jointGroup},
                    event.startTime,
                    context.transitionTime,
                    event.onTransitionEnd,
                    convert(context.currentPose)};

                currentGroups.erase(currentGroupsIter);
                transtionGroups.insert({*event.jointGroupName, newGroup});
            }
            else
            {
                addTransitionBasedOnEvent(event);
            }
        }
    }
    else
    {
        context.machine.transitionTo<AnimationTransition>(context, event.info, event.startTime, event.onTransitionEnd);
    }
}

void AnimationTransitionToMixed::handle(const StopAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = transtionGroups.find(*event.jointGroupName);

        if (iter != transtionGroups.end())
        {
            transtionGroups.erase(iter);
        }
        else
        {
            auto currentGroupsIter = currentGroups.find(*event.jointGroupName);

            if (currentGroupsIter != currentGroups.end())
            {
                currentGroups.erase(currentGroupsIter);
            }
        }

        if (currentGroups.empty() and transtionGroups.size() == 1)
        {
            for (auto &[_, group] : transtionGroups)
            {
                context.machine.transitionTo<AnimationTransition>(context, group.clipInfo, group.time, group.onTransitionEnd);
                return;
            }
        }
        else if (currentGroups.size() == 1 and transtionGroups.empty())
        {
            for (auto &[_, group] : currentGroups)
            {
                context.machine.transitionTo<PlayAnimation>(context, group.clipInfo, group.time);
                return;
            }
        }
        else if (currentGroups.empty() and transtionGroups.empty())
        {
            context.machine.transitionTo<EmptyState>(context);
        }
    }
    else
    {
        context.machine.transitionTo<EmptyState>(context);
    }
}

std::vector<std::string> AnimationTransitionToMixed::getCurrentAnimation() const
{
    std::vector<std::string> r;
    for (auto &[_, group] : currentGroups)
        r.push_back(group.clipInfo.clip.getName());

    for (auto &[_, group] : transtionGroups)
        r.push_back(group.clipInfo.clip.getName());
    return r;
}

bool AnimationTransitionToMixed::isAnimationPlaying(const std::string &name) const
{
    return Utils::contains(currentGroups, [&name](const auto &pair) { return (pair.second.clipInfo.clip.getName() == name); });
}

void AnimationTransitionToMixed::increaseAnimationTime(float deltaTime)
{
    for (auto iter = currentGroups.begin(); iter != currentGroups.end();)
    {
        auto &group     = iter->second;
        auto length     = group.clipInfo.clip.GetLength();
        auto &progress  = group.time;
        float direction = group.clipInfo.playDirection == PlayDirection::forward ? 1.f : -1.f;

        progress += deltaTime * group.clipInfo.playSpeed * direction;

        notifyFrameSubsribers(group.clipInfo, group.frames.first, progress, group.previousFrameTimeStamp);

        if (progress > length)
        {
            if (group.clipInfo.playType == Components::AnimationClipInfo::PlayType::once)
            {
                iter = currentGroups.erase(iter);
                continue;
            }

            progress = fmodf(progress, length);
        }
        if (progress < 0)
        {
            progress = group.clipInfo.clip.GetLength() + progress;
        }

        ++iter;
    }

    if (currentGroups.empty() and transtionGroups.size() == 1)
    {
        for (auto &[_, group] : transtionGroups)
        {
            context.machine.transitionTo<AnimationTransition>(context, group.clipInfo, group.time);
            return;
        }
    }
}

void AnimationTransitionToMixed::increaseTransitionTime(float deltaTime)
{
    for (auto iter = transtionGroups.begin(); iter != transtionGroups.end();)
    {
        auto &group = iter->second;
        group.time += (1.f / group.timeForChange) * deltaTime;

        if (group.time > 1.f)
        {
            PlayGroupMixed newGroup{{{.time = 0.f, .clipInfo = group.clipInfo, .jointGroup = group.jointGroup},
                                     group.clipInfo.playDirection == PlayDirection::forward ? 1.f : -1.f}};

            currentGroups.insert({iter->first, newGroup});

            if (group.onTransitionEnd)
            {
                group.onTransitionEnd();
            }
            iter = transtionGroups.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if (transtionGroups.empty())
    {
        AnimationClipInfoPerGroup infoPerGroup;
        for (const auto &[name, group] : currentGroups)
        {
            infoPerGroup.insert({name, {group.clipInfo, group.time}});
        }
        context.machine.transitionTo<PlayMixedAnimation>(context, infoPerGroup);
    }
}

void AnimationTransitionToMixed::addTransitionBasedOnEvent(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = context.jointGroups.find(*event.jointGroupName);
        if (iter != context.jointGroups.end())
        {
            transtionGroups.insert(
                {*event.jointGroupName, TransitionGroup{{.time = 0.f, .clipInfo = event.info, .jointGroup = &iter->second},
                                                        event.startTime,
                                                        context.transitionTime,
                                                        event.onTransitionEnd,
                                                        convert(context.currentPose)}});
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
