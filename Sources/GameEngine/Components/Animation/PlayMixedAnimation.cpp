#include "PlayMixedAnimation.h"

#include "AnimationTransitionGrouped.h"
#include "EmptyState.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "PlayAnimation.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayMixedAnimation::PlayMixedAnimation(Context& context)
    : context_{context}
    , animationName_{"mixed"}
{
    const std::unordered_map<std::string, std::pair<AnimationClipInfo, std::vector<uint32>>>
        animationPlayingInfoPerGroup_;

//    for (auto& [name, pair] : animationPlayingInfoPerGroup_)
//    {
//        float playingSpeed{info.playDirection_ == PlayDirection::forward ? fabsf(info.playSpeed_)
//                                                                         : -fabsf(info.playSpeed_)};
//        groups_.insert({info.clip, 0.f, playingSpeed, info.endCallbacks_, group});
//        animationName_ += "_" + info.clip.name;
//    }
}
bool PlayMixedAnimation::update(float deltaTime)
{
    for (auto& [_, group] : groups_)
    {
        calculateCurrentAnimationPose(context_.currentPose, group.clip_, group.time_, group.joints_);
    }

    increaseAnimationTime(deltaTime);
    return true;
}
const std::string& PlayMixedAnimation::getAnimationClipName() const
{
    return animationName_;
}

void PlayMixedAnimation::handle(const ChangeAnimationEvent&)
{
}

void PlayMixedAnimation::handle(const StopAnimationEvent& event)
{
    if (not event.jointGroupName)
        context_.machine.transitionTo(std::make_unique<EmptyState>());
}
void PlayMixedAnimation::increaseAnimationTime(float deltaTime)
{
    bool allClipsArePlayedOnce = true;
    for (auto& [name, group] : groups_)
    {
        group.time_ += deltaTime * group.playingSpeed_;

        if (group.time_ > group.clip_.GetLength())
        {
            if (group.clip_.playType == Animation::AnimationClip::PlayType::once)
            {
                continue;
            }
            group.time_ = fmodf(group.time_, group.clip_.GetLength());
        }
        if (group.time_ < 0)
        {
            group.time_ = group.clip_.GetLength() + group.time_;
        }

        allClipsArePlayedOnce = false;
    }

    if (allClipsArePlayedOnce)
    {
        context_.machine.transitionTo(std::make_unique<EmptyState>());
    }
    else
    {
        context_.machine.transitionTo(std::make_unique<AnimationTransitionGrouped>());
    }
}
}  // namespace Components
}  // namespace GameEngine
