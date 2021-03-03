#include "PlayMixedAnimation.h"

#include "EndAnimationTransitionEvent.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
PlayMixedAnimation::PlayMixedAnimation(const PlayMixedAnimationEvent& event)
    : machine_{*event.machine}
    , currentPose_{event.currentPose}
    , animationName_{"mixed"}
{
    groups_.reserve(event.animationPlayingInfoPerGroup_.size());
    for (auto& [info, group] : event.animationPlayingInfoPerGroup_)
    {
        float playingSpeed{info.playDirection_ == PlayDirection::forward ? fabsf(info.playSpeed_)
                                                                         : -fabsf(info.playSpeed_)};
        groups_.push_back({info.clip, 0.f, playingSpeed, info.endCallbacks_, group});
        animationName_ += "_" + info.clip.name;
    }
}
void PlayMixedAnimation::update(float deltaTime)
{
    for (auto& group : groups_)
    {
        calculateCurrentAnimationPose(currentPose_, group.clip_, group.time_, group.joints_);
    }

    increaseAnimationTime(deltaTime);
}
const std::string& PlayMixedAnimation::getAnimationClipName() const
{
    return animationName_;
}
void PlayMixedAnimation::increaseAnimationTime(float deltaTime)
{
    for (auto& group : groups_)
    {
        group.time_ += deltaTime * group.playingSpeed_;

        if (group.time_ > group.clip_.GetLength())
        {
            // if (clip_.playType == Animation::AnimationClip::PlayType::once)
            //{
            //    machine_.handle(std::make_unique<EndAnimationTransitionEvent>(endCallbacks_));
            //    return;
            //}

            group.time_ = fmodf(group.time_, group.clip_.GetLength());
        }
        if (group.time_ < 0)
        {
            group.time_ = group.clip_.GetLength() + group.time_;
        }
    }
}
}  // namespace Components
}  // namespace GameEngine
