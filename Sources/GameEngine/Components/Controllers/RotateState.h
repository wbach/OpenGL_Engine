#pragma once
#include <Logger/Log.h>
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class MoveAndRotateState;
class JumpState;
class IdleState;
class DeathState;

class RotateState
    : public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<IdleState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    RotateState(FsmContext& context)
        : RotateStateBase{context}
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
