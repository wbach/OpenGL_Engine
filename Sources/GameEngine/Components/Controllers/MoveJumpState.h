#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "JumpStateBase.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class JumpState;
class DeathState;

class MoveJumpState : public JumpStateBase,
                      public Utils::StateMachine::Will<
                          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>,
                          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                          Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<JumpState>>,
                          Utils::StateMachine::On<EndJumpEvent, Utils::StateMachine::TransitionTo<MoveState>>>
{
public:
    MoveJumpState(FsmContext& context, std::function<void()> endCallback)
        : JumpStateBase(context, endCallback)
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
