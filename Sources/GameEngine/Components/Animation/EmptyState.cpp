#include "EmptyState.h"

#include "AnimationTransition.h"
#include "StateMachine.h"
#include <Logger/Log.h>

namespace GameEngine
{
namespace Components
{
EmptyState::EmptyState(Context &context)
    : context_{context}
{
}
bool EmptyState::update(float)
{
    return false;
}
void EmptyState::handle(const ChangeAnimationEvent &event)
{
    auto action = std::make_unique<AnimationTransition>(context_, event.info, event.startTime);
    context_.machine.transitionTo(std::move(action));
}
void EmptyState::handle(const StopAnimationEvent &)
{
}

std::vector<std::string> EmptyState::getCurrentAnimation() const
{
    return {};
}
}  // namespace Components
}  // namespace GameEngine
