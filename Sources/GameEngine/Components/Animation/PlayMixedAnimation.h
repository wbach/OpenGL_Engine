#pragma once
#include "GameEngine/Animations/AnimationClip.h"
#include "IState.h"
#include "PlayMixedAnimationEvent.h"

namespace GameEngine
{
namespace Components
{
class StateMachine;

class PlayMixedAnimation : public IState
{
public:
    PlayMixedAnimation(const PlayMixedAnimationEvent&);
    void update(float) override;
    const std::string& getAnimationClipName() const override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    StateMachine& machine_;
    Pose& currentPose_;

    struct Group
    {
        const Animation::AnimationClip& clip_;
        float time_;
        float playingSpeed_;
        const std::vector<std::function<void()>>& endCallbacks_;
        std::vector<uint32> joints_;
    };

    std::string animationName_;
    std::vector<Group> groups_;
};
}  // namespace Components
}  // namespace GameEngine
