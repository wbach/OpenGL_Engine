#include "EndAnimationTransitionEvent.h"

#include "IState.h"
#include "StateMachine.h"

namespace GameEngine
{
namespace Components
{
EndAnimationTransitionEvent::EndAnimationTransitionEvent(const std::vector<std::function<void()>>& callbacks)
    : onEntryCallbacks{callbacks}
{
}
void EndAnimationTransitionEvent::process()
{
    machine->currentState_.reset(nullptr);

    for (const auto& callback : onEntryCallbacks)
    {
        callback();
    }
}
}  // namespace Components
}  // namespace GameEngine
