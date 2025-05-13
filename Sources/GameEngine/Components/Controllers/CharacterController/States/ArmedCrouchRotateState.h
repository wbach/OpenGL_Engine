#pragma once
#include "CharacterControllerCommonDefs.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedCrouchRotateState
    : public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedCrouchRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedCrouchRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedCrouchRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<ArmedIdleCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchRotateState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<DodgeDiveEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeForwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeBackwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeLeftEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeRightEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedCrouchRotateState(FsmContext&);

    using RotateStateBase::onEnter;
    using RotateStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
