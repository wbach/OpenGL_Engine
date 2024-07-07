#pragma once
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedAttackAndRotateState
    : public AttackStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRotateState>>,
          Utils::StateMachine::On<MoveForwardEvent,
                                  Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent,
                                  Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<MoveRightEvent,
                                  Utils::StateMachine::TransitionTo<DisarmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DisarmedAttackState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>>
{
public:
    DisarmedAttackAndRotateState(FsmContext&);

    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
