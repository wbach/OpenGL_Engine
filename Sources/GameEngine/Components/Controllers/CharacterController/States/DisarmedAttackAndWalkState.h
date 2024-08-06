#pragma once
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class DisarmedAttackAndWalkState
    : public AttackStateBase,
      public RotatingMoveState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<ChangeAnimEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::RemoveFromQueue<DrawArrowEvent>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DisarmedAttackAndRunState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedAttackState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedAttackState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<DisarmedAttackState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<DisarmedAttackState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>>
{
public:
    DisarmedAttackAndWalkState(FsmContext&);

    using AttackStateBase::onEnter;
    using AttackStateBase::onLeave;
    using AttackStateBase::update;

    using RotatingMoveState::onEnter;
    using RotatingMoveState::update;

    void update(float);

    void onMoveInactivity() override;
};
}  // namespace Components
}  // namespace GameEngine
