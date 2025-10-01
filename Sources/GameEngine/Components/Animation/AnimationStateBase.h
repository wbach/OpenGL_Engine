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
struct Context;
struct AnimationClipInfo;

class AnimationStateBase : public IAnimationState
{
protected:
    AnimationStateBase(Context& context);
    struct AnimationGroup
    {
        float time{0.f};
        const AnimationClipInfo& clipInfo;
        const std::vector<uint32>* jointGroup{nullptr};
    };

    struct PlayGroup : public AnimationGroup
    {
        float direction{1.f};
        float previousFrameTimeStamp{-1.0f};
    };

    struct PlayGroupMixed : public PlayGroup
    {
        CurrentFrames frames{nullptr, nullptr};
    };

    struct TransitionGroup : public AnimationGroup
    {
        float startupTime{0.f};
        float timeForChange{0.5f};
        std::function<void()> onTransitionEnd;
        Animation::KeyFrame startChaneAnimKeyFrame;
        Animation::KeyFrame endChangeAnimKeyFrame;
    };

    void notifyFrameSubsribers(const AnimationClipInfo&, const Animation::KeyFrame*, float, float&);
    bool increaseAnimationTime(float&, float&, const AnimationClipInfo&, Animation::KeyFrame const*, float);
    bool isAnimationPlaying(const std::string&) const override;

    void calculateRootMontionVecAndClearTranslation();
    Context& context_;

private:
    std::optional<vec3> rootMontionVec_;
};
}  // namespace Components
}  // namespace GameEngine
