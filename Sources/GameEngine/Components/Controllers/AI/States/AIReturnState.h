#pragma once
#include <Utils/Fsm/Actions.h>

#include "AIStateBase.h"

namespace GameEngine
{
namespace Components
{
class AIAmbientState;

class AIReturnState : public AIStateBase,
                      public Utils::StateMachine::Will<
                          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                          Utils::StateMachine::On<NavigationPathCompletedEvent, Utils::StateMachine::TransitionTo<AIAmbientState>>>
{
public:
    AIReturnState(AIControllerContext&);

    using AIStateBase::onEnter;
    using AIStateBase::onLeave;

    void onEnter(const TargetLostEvent&);
    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
