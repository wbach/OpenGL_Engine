#pragma once
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
class DisarmedAttackState
    : public AttackStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<ChangeAnimEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    DisarmedAttackState(FsmContext&);

    using AttackStateBase::onEnter;
    using AttackStateBase::onLeave;
    using AttackStateBase::update;

    using MaybeAttackStates = Utils::StateMachine::Maybe<Utils::StateMachine::Update,
                                                         Utils::StateMachine::TransitionTo<DisarmedAttackAndWalkState>,
                                                         Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>>;

    MaybeAttackStates handle(const MoveForwardEvent&);
    MaybeAttackStates handle(const MoveBackwardEvent&);
    MaybeAttackStates handle(const MoveLeftEvent&);
    MaybeAttackStates handle(const MoveRightEvent&);

    using Will::handle;

private:
    template <typename Event>
    MaybeAttackStates handleMoveEvents(const Event&);
};
}  // namespace Components
}  // namespace GameEngine
