#include "AnimationTransitionToMixed.h"

#include <Logger/Log.h>

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

AnimationTransitionToMixed::AnimationTransitionToMixed(Context &context,
                                                       const TransitionGroupsPlayings &transitionGroupsPlaying,
                                                       const ChangeAnimationEvent &event)
    : AnimationTransitionToMixed(context, {}, transitionGroupsPlaying, event)
{
}

AnimationTransitionToMixed::AnimationTransitionToMixed(Context &context,
                                                       const CurrentGroupsPlayingInfos &currentGroupsPlayingInfos,
                                                       const TransitionGroupsPlayings &transitionGroupsPlaying,
                                                       const ChangeAnimationEvent &event)
    : context_{context}
{
    for (auto &info : currentGroupsPlayingInfos)
    {
        for (auto &groupName : info.jointGroupNames)
        {
            auto iter = context.jointGroups.find(groupName);

            if (iter != context_.jointGroups.end())
            {
                currentGroups_.insert({groupName, {info.info, info.currentTime, iter->second}});
            }
        }
    }

    for (auto &info : transitionGroupsPlaying)
    {
        for (auto &groupName : info.jointGroupNames)
        {
            auto iter = context.jointGroups.find(groupName);

            if (iter != context_.jointGroups.end())
            {
                transtionGroups_.insert({groupName, TransitionGroup{0.f, info.info, iter->second, info.onTransitionEnd,
                                                                    convert(context_.currentPose),
                                                                    context_.transitionTime, info.currentTime}});
            }
        }
    }

    addTransitionBasedOnEvent(event);
}

bool AnimationTransitionToMixed::update(float deltaTime)
{
    for (auto &[name, group] : currentGroups_)
    {
        if (not group.jointGroup_.empty())
        {
            calculateCurrentAnimationPose(context_.currentPose, group.clipInfo_.clip, group.time_, group.jointGroup_);
            group.frames = context_.currentPose.frames;
        }
    }

    for (const auto &[name, group] : transtionGroups_)
    {
        const auto &endChangeAnimKeyFrame = group.clipInfo_.clip.GetFrames().front();
        interpolatePoses(context_.currentPose, group.startKeyFrame_, endChangeAnimKeyFrame, group.progress_,
                         group.jointGroup_);
    }

    increaseAnimationTime(deltaTime);
    increaseTransitionTime(deltaTime);
    return true;
}

void AnimationTransitionToMixed::handle(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = transtionGroups_.find(*event.jointGroupName);

        if (iter != transtionGroups_.end())
        {
            TransitionGroup newGroup{event.startTime,
                                     event.info,
                                     iter->second.jointGroup_,
                                     event.onTransitionEnd,
                                     convert(context_.currentPose),
                                     context_.transitionTime,
                                     0.f};

            transtionGroups_.erase(iter);
            transtionGroups_.insert({*event.jointGroupName, newGroup});
        }
        else
        {
            auto currentGroupsIter = currentGroups_.find(*event.jointGroupName);

            if (currentGroupsIter != currentGroups_.end())
            {
                TransitionGroup newGroup{event.startTime,
                                         event.info,
                                         currentGroupsIter->second.jointGroup_,
                                         event.onTransitionEnd,
                                         convert(context_.currentPose),
                                         context_.transitionTime,
                                         0.f};

                currentGroups_.erase(currentGroupsIter);
                transtionGroups_.insert({*event.jointGroupName, newGroup});
            }
            else
            {
                addTransitionBasedOnEvent(event);
            }
        }
    }
    else
    {
        context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime,
                                                           event.onTransitionEnd);
    }
}

void AnimationTransitionToMixed::handle(const StopAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = transtionGroups_.find(*event.jointGroupName);

        if (iter != transtionGroups_.end())
        {
            transtionGroups_.erase(iter);
        }
        else
        {
            auto currentGroupsIter = currentGroups_.find(*event.jointGroupName);

            if (currentGroupsIter != currentGroups_.end())
            {
                currentGroups_.erase(currentGroupsIter);
            }
        }

        if (currentGroups_.empty() and transtionGroups_.size() == 1)
        {
            for (auto &[_, group] : transtionGroups_)
            {
                context_.machine.transitionTo<AnimationTransition>(context_, group.clipInfo_, group.progress_,
                                                                   group.onTransitionEnd_);
                return;
            }
        }
        else if (currentGroups_.size() == 1 and transtionGroups_.empty())
        {
            for (auto &[_, group] : currentGroups_)
            {
                context_.machine.transitionTo<PlayAnimation>(context_, group.clipInfo_, group.time_);
                return;
            }
        }
        else if (currentGroups_.empty() and transtionGroups_.empty())
        {
            context_.machine.transitionTo<EmptyState>(context_);
        }
    }
    else
    {
        context_.machine.transitionTo<EmptyState>(context_);
    }
}

std::vector<std::string> AnimationTransitionToMixed::getCurrentAnimation() const
{
    std::vector<std::string> r;
    for (auto &[_, group] : currentGroups_)
        r.push_back(group.clipInfo_.clip.name);

    for (auto &[_, group] : transtionGroups_)
        r.push_back(group.clipInfo_.clip.name);
    return r;
}

bool AnimationTransitionToMixed::isAnimationPlaying(const std::string &name) const
{
    auto iter = std::find_if(currentGroups_.begin(), currentGroups_.end(),
                             [&name](const auto &pair) { return (pair.second.clipInfo_.clip.name == name); });
    return iter != currentGroups_.end();
}

void AnimationTransitionToMixed::increaseAnimationTime(float deltaTime)
{
    for (auto iter = currentGroups_.begin(); iter != currentGroups_.end();)
    {
        auto &group     = iter->second;
        auto length     = group.clipInfo_.clip.GetLength();
        auto &progress  = group.time_;
        float direction = group.clipInfo_.playDirection == PlayDirection::forward ? 1.f : -1.f;

        progress += deltaTime * group.clipInfo_.playSpeed * direction;

        notifyFrameSubsribers(group.clipInfo_, group.frames.first, progress, group.previousFrameTimeStamp);

        if (progress > length)
        {
            if (group.clipInfo_.clip.playType == Animation::AnimationClip::PlayType::once)
            {
                iter = currentGroups_.erase(iter);
                continue;
            }

            progress = fmodf(progress, length);
        }
        if (progress < 0)
        {
            progress = group.clipInfo_.clip.GetLength() + progress;
        }

        ++iter;
    }

    if (currentGroups_.empty() and transtionGroups_.size() == 1)
    {
        for (auto &[_, group] : transtionGroups_)
        {
            context_.machine.transitionTo<AnimationTransition>(context_, group.clipInfo_, group.progress_);
            return;
        }
    }
}

void AnimationTransitionToMixed::increaseTransitionTime(float deltaTime)
{
    for (auto iter = transtionGroups_.begin(); iter != transtionGroups_.end();)
    {
        auto &group = iter->second;
        group.progress_ += (1.f / group.timeForChange_) * deltaTime;

        if (group.progress_ > 1.f)
        {
            Group newGroup{group.clipInfo_, 0.f, group.jointGroup_};
            currentGroups_.insert({iter->first, newGroup});

            if (group.onTransitionEnd_)
            {
                group.onTransitionEnd_();
            }
            iter = transtionGroups_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    if (transtionGroups_.empty())
    {
        AnimationClipInfoPerGroup infoPerGroup;
        for (const auto &[name, group] : currentGroups_)
        {
            infoPerGroup.insert({name, {group.clipInfo_, group.time_}});
        }
        context_.machine.transitionTo<PlayMixedAnimation>(context_, infoPerGroup);
    }
}

void AnimationTransitionToMixed::addTransitionBasedOnEvent(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = context_.jointGroups.find(*event.jointGroupName);
        if (iter != context_.jointGroups.end())
        {
            transtionGroups_.insert({*event.jointGroupName,
                                     TransitionGroup{event.startTime, event.info, iter->second, event.onTransitionEnd,
                                                     convert(context_.currentPose), context_.transitionTime, 0.f}});
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
