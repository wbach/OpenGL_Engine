#pragma once
#include "IState.h"
#include "Context.h"

namespace GameEngine
{
namespace Components
{
class StateMachine;

struct StopAnmiationEvent;

class EmptyState : public IState
{
public:
    EmptyState(Context&);
    bool update(float) override;
    const std::string& getAnimationClipName() const override;
    void handle(const ChangeAnimationEvent&) override;
    void handle(const StopAnimationEvent&) override;

private:
    Context& context_;
};
}  // namespace Components
}  // namespace GameEngine
