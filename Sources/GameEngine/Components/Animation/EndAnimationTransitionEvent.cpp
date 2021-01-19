#include "EndAnimationTransitionEvent.h"
#include "StateMachine.h"
#include "IState.h"

namespace GameEngine
{
namespace Components
{
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
