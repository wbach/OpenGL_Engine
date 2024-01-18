#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedRunAndRotateState;
class JumpState;
class DisarmedIdleState;
class MoveJumpState;
class DeathState;
class ArmedRunState;

class DisarmedRunState
    : public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    DisarmedRunState(FsmContext& context)
        : MoveStateBase{context, context.runSpeed, context.animClipNames.disarmed.run.forward,
                        context.animClipNames.disarmed.run.backward}
    {
    }

    using MoveStateBase::onEnter;

    void onEnter(const WeaponStateEvent&)
    {
        DEBUG_LOG("void onEnter(const WeaponStateEvent&) dir=" + std::to_string(context_.moveDirection));
        context_.multiAnimations = true;
        StateBase::disarmWeapon();
        MoveStateBase::setCurrentAnim();
    }
};
}  // namespace Components
}  // namespace GameEngine
