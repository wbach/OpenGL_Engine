#include "EmptyState.h"

#include <Logger/Log.h>

#include "AnimationTransition.h"
#include "StateMachine.h"

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
    context_.machine.transitionTo<AnimationTransition>(context_, event.info, event.startTime, event.onTransitionEnd);
}
void EmptyState::handle(const StopAnimationEvent &)
{
    DEBUG_LOG("EmptyState StopAnimationEvent?");
}

std::vector<std::string> EmptyState::getCurrentAnimation() const
{
    return {};
}
}  // namespace Components
}  // namespace GameEngine
