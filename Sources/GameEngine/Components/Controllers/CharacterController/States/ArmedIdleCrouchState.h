#pragma once
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedIdleCrouchState
    : public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<ArmedAttackState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<MoveEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>,
//          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
//          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
//          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
//          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedIdleCrouchState(FsmContext&);

    using IdleStateBase::onEnter;
    using IdleStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
