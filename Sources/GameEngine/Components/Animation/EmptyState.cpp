#include "EmptyState.h"

#include "AnimationTransition.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const std::string clipName = "emptyState";
}

EmptyState::EmptyState(Context &context)
    : context_{context}
{
}

bool EmptyState::update(float)
{
    return false;
}

const std::string &EmptyState::getAnimationClipName() const
{
    return clipName;
}

void EmptyState::handle(const ChangeAnimationEvent &event)
{
    auto action = std::make_unique<AnimationTransition>(context_, event.startTime, event.info);
    context_.machine.transitionTo(std::move(action));
}

void EmptyState::handle(const StopAnimationEvent &)
{
}

}  // namespace Components
}  // namespace GameEngine
