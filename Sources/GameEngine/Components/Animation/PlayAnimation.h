#pragma once
#include "GameEngine/Animations/AnimationClip.h"
#include "AnimationStateBase.h"


namespace GameEngine
{
namespace Components
{
struct StateMachine;
struct StopAnmiationEvent;

class PlayAnimation : public AnimationStateBase
{
public:
    PlayAnimation(Context&, const AnimationClipInfo&, float);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;

    bool isAnimationPlaying(const std::string&) const override;

private:
    PlayGroup playInfo_;
};
}  // namespace Components
}  // namespace GameEngine
