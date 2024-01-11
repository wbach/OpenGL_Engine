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
AnimationTransitionToMixed::AnimationTransitionToMixed(
    Context &context, const std::vector<CurrentGroupsPlayingInfo> &currentGroupsPlayingInfos,
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

    if (event.jointGroupName)
    {
        auto iter = context_.jointGroups.find(*event.jointGroupName);
        if (iter != context_.jointGroups.end())
        {
            transtionGroups_.insert(
                {*event.jointGroupName, TransitionGroup{event.startTime, event.info, iter->second,
                                                        convert(context_.currentPose), context_.transitionTime, 0.f}});
        }
    }
}

bool AnimationTransitionToMixed::update(float deltaTime)
{
    for (const auto &[_, group] : currentGroups_)
    {
        if (not group.jointGroup_.empty())
        {
            calculateCurrentAnimationPose(context_.currentPose, group.clipInfo_.clip, group.progres_,
                                          group.jointGroup_);
        }
    }

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
    if (event.jointGroupName)
    {
        auto iter = transtionGroups_.find(*event.jointGroupName);

        if (iter != transtionGroups_.end())
        {
            TransitionGroup newGroup{event.startTime,          event.info,
                                     iter->second.jointGroup_, convert(context_.currentPose),
                                     context_.transitionTime,  0.f};

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
                                         convert(context_.currentPose),
                                         context_.transitionTime,
                                         0.f};

                currentGroups_.erase(currentGroupsIter);
                transtionGroups_.insert({*event.jointGroupName, newGroup});
            }
        }
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

void AnimationTransitionToMixed::increaseAnimationTime(float deltaTime)
{
    for (auto iter = currentGroups_.begin(); iter != currentGroups_.end();)
    {
        auto& group = iter->second;
        auto length    = group.clipInfo_.clip.GetLength();
        auto &progress = group.progres_;

        progress += deltaTime * group.clipInfo_.playSpeed;  //* direction_;

        if (progress > length)
        {
            if (group.clipInfo_.clip.playType == Animation::AnimationClip::PlayType::once)
            {
//                for (const auto& [_, callback] : group.clipInfo_.endCallbacks_)
//                {
//                    callback();
//                }

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

//    if (not context_.machine.queueEvents_.empty())
//        return;

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
            infoPerGroup.insert({name, {group.clipInfo_, group.progres_}});
        }
        context_.machine.transitionTo(std::make_unique<PlayMixedAnimation>(context_, infoPerGroup));
    }
}
}  // namespace Components
}  // namespace GameEngine
