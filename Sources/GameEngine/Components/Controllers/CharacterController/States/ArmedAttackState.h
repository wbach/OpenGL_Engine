#pragma once
#include "../PlayStateType.h"
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
class ArmedAttackState
    : public AttackStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRotateState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<DodgeDiveEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeForwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeBackwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeLeftEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeRightEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedAttackState(FsmContext&);

    using AttackStateBase::onEnter;
    using AttackStateBase::onLeave;
    using AttackStateBase::update;

    using MaybeAttackStates =
        Utils::StateMachine::Maybe<Utils::StateMachine::Update, Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>,
                                   Utils::StateMachine::TransitionTo<ArmedAttackAndRunState>>;

    MaybeAttackStates handle(const MoveForwardEvent&);
    MaybeAttackStates handle(const MoveBackwardEvent&);
    MaybeAttackStates handle(const MoveLeftEvent&);
    MaybeAttackStates handle(const MoveRightEvent&);

    MaybeAttackStates handle(const ChangeAnimEvent&);

    using Will::handle;

private:
    template <typename Event>
    MaybeAttackStates handleMoveEvents(const Event&);
    MaybeAttackStates getCorrespodingState(PlayStateType);
};
}  // namespace Components
}  // namespace GameEngine
