#include "AnimationTransitionToMixed.h"

#include <Logger/Log.h>

#include <algorithm>

#include "AnimationTransition.h"
#include "EmptyState.h"
#include "PlayAnimation.h"
#include "PlayMixedAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
AnimationTransitionToMixed::AnimationTransitionToMixed(Context &context,
                                                       const std::vector<CurrentGroupsPlayingInfo> &current,
                                                       const ChangeAnimationEvent &event,
                                                       std::function<void()> onTransitionEnd)
    : AnimationTransitionToMixed(context, current, {}, event, onTransitionEnd)
{
}

AnimationTransitionToMixed::AnimationTransitionToMixed(
    Context &context, const std::vector<TransitionGroupsPlaying> &transitionGroupsPlaying,
    const ChangeAnimationEvent &event, std::function<void()> onTransitionEnd)
    : AnimationTransitionToMixed(context, {}, transitionGroupsPlaying, event, onTransitionEnd)
{
}

AnimationTransitionToMixed::AnimationTransitionToMixed(
    Context &context, const std::vector<CurrentGroupsPlayingInfo> &currentGroupsPlayingInfos,
    const std::vector<TransitionGroupsPlaying> &transitionGroupsPlaying, const ChangeAnimationEvent &event,
    std::function<void()> onTransitionEnd)
    : context_{context}
{
    DEBUG_LOG("AnimationTransitionToMixed currentGroupsPlayingInfos.size=" +
              std::to_string(currentGroupsPlayingInfos.size()));
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
    DEBUG_LOG("AnimationTransitionToMixed transitionGroupsPlaying.size=" +
              std::to_string(transitionGroupsPlaying.size()));

    for (auto &info : transitionGroupsPlaying)
    {
        for (auto &groupName : info.jointGroupNames)
        {
            auto iter = context.jointGroups.find(groupName);

            if (iter != context_.jointGroups.end())
            {
                DEBUG_LOG("add " + groupName);
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
    DEBUG_LOG("AnimationTransitionToMixed currentGroups_.size=" + std::to_string(currentGroups_.size()));
    for (auto &[_, group] : currentGroups_)
    {
        if (not group.jointGroup_.empty())
        {
            calculateCurrentAnimationPose(context_.currentPose, group.clipInfo_.clip, group.time_, group.jointGroup_);
            group.frames = context_.currentPose.frames;
        }
    }
    DEBUG_LOG("AnimationTransitionToMixed transtionGroups_.size=" + std::to_string(transtionGroups_.size()));

    for (const auto &[_, group] : transtionGroups_)
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
    DEBUG_LOG(event.info.clip.name);

    if (event.jointGroupName)
    {
        DEBUG_LOG(*event.jointGroupName);
        auto iter = transtionGroups_.find(*event.jointGroupName);

        if (iter != transtionGroups_.end())
        {
            DEBUG_LOG("Group found");
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
            DEBUG_LOG("Group no found");
            auto currentGroupsIter = currentGroups_.find(*event.jointGroupName);

            if (currentGroupsIter != currentGroups_.end())
            {
                DEBUG_LOG("Group found in current");
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
        context_.machine.transitionTo(
            std::make_unique<AnimationTransition>(context_, event.info, event.startTime, event.onTransitionEnd));
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
                context_.machine.transitionTo(
                    std::make_unique<AnimationTransition>(context_, group.clipInfo_, group.progress_));
                return;
            }
        }
        else if (currentGroups_.size() == 1 and transtionGroups_.empty())
        {
            for (auto &[_, group] : transtionGroups_)
            {
                context_.machine.transitionTo(
                    std::make_unique<AnimationTransition>(context_, group.clipInfo_, group.progress_));
                return;
            }
        }
        else if (currentGroups_.empty() and transtionGroups_.empty())
        {
            context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
        }
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<EmptyState>(context_));
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
            context_.machine.transitionTo(
                std::make_unique<AnimationTransition>(context_, group.clipInfo_, group.progress_));
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
        context_.machine.transitionTo(std::make_unique<PlayMixedAnimation>(context_, infoPerGroup));
    }
}

void AnimationTransitionToMixed::addTransitionBasedOnEvent(const ChangeAnimationEvent &event)
{
    if (event.jointGroupName)
    {
        auto iter = context_.jointGroups.find(*event.jointGroupName);
        if (iter != context_.jointGroups.end())
        {
            DEBUG_LOG("add " + (*event.jointGroupName));
            transtionGroups_.insert({*event.jointGroupName,
                                     TransitionGroup{event.startTime, event.info, iter->second, event.onTransitionEnd,
                                                     convert(context_.currentPose), context_.transitionTime, 0.f}});
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
