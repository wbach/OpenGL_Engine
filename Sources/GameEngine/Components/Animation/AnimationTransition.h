#pragma once
#include "IState.h"
#include "AnimationPlayingInfo.h"

namespace GameEngine
{
namespace Components
{
struct AnimationTransitionEvent;
struct StateMachine;

class AnimationTransition : public IState
{
public:
    AnimationTransition(const AnimationTransitionEvent&);
    void update(float) override;
    const std::string& getAnimationClipName() const override;

private:
    void calculateTime(float deltaTime);

private:
    StateMachine& machine_;
    const AnimationPlayingInfo info_;
    Pose& currentPose_;

    Animation::KeyFrame startChaneAnimKeyFrame_;
    Animation::KeyFrame endChangeAnimKeyFrame_;

    float timeForChange_;
    float currentTime_;
};
}  // namespace Components
}  // namespace GameEngine
