#pragma once
#include "IAnimationState.h"

namespace GameEngine
{
namespace Animation
{
struct KeyFrame;
}  // namespace Animation
namespace Components
{
class AnimationClipInfo;

class AnimationStateBase : public IAnimationState
{
protected:
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

    struct TransitionGroup
    {
        Core core;
        float startupTime;
        float timeForChange;
        std::function<void()> onTransitionEnd;
        Animation::KeyFrame startKeyFram_;
    };

    void notifyFrameSubsribers(const AnimationClipInfo&, const Animation::KeyFrame*, float, float&);
    bool increaseAnimationTime(float&, float&, const AnimationClipInfo&, Animation::KeyFrame const*, float);
};
}  // namespace Components
}  // namespace GameEngine
