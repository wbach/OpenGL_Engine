#pragma once
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
          Utils::StateMachine::On<ChangeAnimEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          //           Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          //           Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          //           Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedAttackState(FsmContext&);

    using AttackStateBase::onEnter;
    using AttackStateBase::onLeave;
    using AttackStateBase::update;

    using MaybeAttackStates = Utils::StateMachine::Maybe<Utils::StateMachine::Update,
                                                         Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>,
                                                         Utils::StateMachine::TransitionTo<ArmedAttackAndRunState>>;

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
