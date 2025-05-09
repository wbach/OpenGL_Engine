#pragma once
#include "AnimationStateBase.h"
#include "Context.h"

namespace GameEngine
{
namespace Components
{
struct StateMachine;
struct StopAnmiationEvent;

class EmptyState : public AnimationStateBase
{
public:
    EmptyState(Context&);
    bool update(float) override;

    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

    std::vector<std::string> getCurrentAnimation() const override;
};
}  // namespace Components
}  // namespace GameEngine
