#pragma once
#include "GameEngine/Animations/AnimationClip.h"
#include "IState.h"
#include "PlayAnimationEvent.h"
#include "Context.h"

namespace GameEngine
{
namespace Components
{
class StateMachine;

struct StopAnmiationEvent;

class PlayAnimation : public IState
{
public:
    PlayAnimation(Context&, const AnimationClipInfo&, float);
    bool update(float) override;
    const std::string& getAnimationClipName() const override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    void increaseAnimationTime(float deltaTime);

private:
    Context& context_;

    float time_;
    float direction_;
    AnimationClipInfo clipInfo_;
};
}  // namespace Components
}  // namespace GameEngine
