#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "MoveStateBase.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunAndRotateState;
class JumpState;
class ArmedIdleState;
class MoveJumpState;
class DeathState;

class ArmedRunState
    : public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          //Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          //Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    ArmedRunState(FsmContext& context)
        : MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward, context.animClipNames.armed.run.backward}
    {
    }
};
}  // namespace Components
}  // namespace GameEngine
