#pragma once
#include <Utils/Fsm/Actions.h>

#include "../ActionAIEvents.h"

namespace GameEngine
{
namespace Components
{
class AIMoveToState;
class AIAmbientState : public Utils::StateMachine::Will<
                           Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                           Utils::StateMachine::On<MoveToTargetEvent, Utils::StateMachine::TransitionTo<AIMoveToState>>>
{
public:
    void onEnter();
    void update(float)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
