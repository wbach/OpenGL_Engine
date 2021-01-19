#pragma once
#include "GameEngine/Animations/AnimationClip.h"
#include "IState.h"
#include "PlayAnimationEvent.h"

namespace GameEngine
{
namespace Components
{
class StateMachine;

class PlayAnimation : public IState
{
public:
    PlayAnimation(const PlayAnimationEvent&);
    Pose update(float) override;
    const std::string& getAnimationClipName() const override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    StateMachine& machine_;
    const Animation::AnimationClip& clip_;
    float time_;
    float playingSpeed_;
    const std::vector<std::function<void()>>& endCallbacks_;
};
}  // namespace Components
}  // namespace GameEngine
