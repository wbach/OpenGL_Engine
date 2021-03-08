#include "EmptyState.h"

#include "AnimationTransition.h"
#include "StateMachine.h"
#include <Logger/Log.h>

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
    DEBUG_LOG("");
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
    DEBUG_LOG("ChangeAnimationEvent");
    auto action = std::make_unique<AnimationTransition>(context_, event.info, event.startTime);
    context_.machine.transitionTo(std::move(action));
}

void EmptyState::handle(const StopAnimationEvent &)
{
}

}  // namespace Components
}  // namespace GameEngine
