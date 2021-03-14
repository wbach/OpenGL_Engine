#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "MoveStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class JumpState;
class DeathState;
struct FsmContext;

class MoveAndRotateState
    : public MoveStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<MoveAndRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<MoveState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    MoveAndRotateState(FsmContext&);

    void onEnter(const AttackEvent&);
    void onEnter(const EndAttackEvent&);
    void onEnter(const EndJumpEvent&);
    void onEnter(const MoveForwardEvent&);
    void onEnter(const MoveBackwardEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);

    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
