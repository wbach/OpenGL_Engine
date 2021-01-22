#include "PlayAnimation.h"

#include "EndAnimationTransitionEvent.h"
#include "GameEngine/Resources/ShaderBuffers/PerPoseUpdate.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
namespace
{
struct TimeSnap
{
    float frameTime;
    std::array<mat4, MAX_BONES> boneTransforms;
};
using ModelId = uint32;
using Clipname = std::string;

std::unordered_map<ModelId, std::unordered_map<Clipname, std::vector<TimeSnap>>> precaculatedAnimationTransforms;
}  // namespace

PlayAnimation::PlayAnimation(const PlayAnimationEvent& event)
    : machine_{*event.machine}
    , clip_{event.animationPlayingInfo_.clip}
    , currentPose_{event.currentPose}
    , time_{0}
    , playingSpeed_{event.animationPlayingInfo_.playDirection_ == PlayDirection::forward
                        ? fabsf(event.animationPlayingInfo_.playSpeed_)
                        : -fabsf(event.animationPlayingInfo_.playSpeed_)}
    , endCallbacks_{event.animationPlayingInfo_.endCallbacks_}
{
}
void PlayAnimation::update(float deltaTime)
{
    calculateCurrentAnimationPose(currentPose_, clip_, time_);
    increaseAnimationTime(deltaTime);
}
const std::string& PlayAnimation::getAnimationClipName() const
{
    return clip_.name;
}
void PlayAnimation::increaseAnimationTime(float deltaTime)
{
    time_ += deltaTime * playingSpeed_;

    if (time_ > clip_.GetLength())
    {
        if (clip_.playType == Animation::AnimationClip::PlayType::once)
        {
            machine_.handle(std::make_unique<EndAnimationTransitionEvent>(endCallbacks_));
            return;
        }
        time_ = fmodf(time_, clip_.GetLength());
    }
    if (time_ < 0)
    {
        time_ = clip_.GetLength() + time_;
    }
}
}  // namespace Components
}  // namespace GameEngine
