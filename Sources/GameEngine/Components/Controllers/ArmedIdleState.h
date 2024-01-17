#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunState;
class ArmedRotateState;
class DisarmedIdleState;
class JumpState;
class DeathState;

class ArmedIdleState
    : public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedIdleState(FsmContext&);

    using IdleStateBase::onEnter;
    using IdleStateBase::update;

    void onEnter(const WeaponStateEvent&);
};
}  // namespace Components
}  // namespace GameEngine
