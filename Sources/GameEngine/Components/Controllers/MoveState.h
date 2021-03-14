#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class MoveAndRotateState;
class JumpState;
class IdleState;
class MoveJumpState;
class DeathState;

class MoveState : public MoveStateBase,
                  public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                      Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<IdleState>>,
                      Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
                      Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
                      Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    MoveState(FsmContext& context)
        : MoveStateBase{context}
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
