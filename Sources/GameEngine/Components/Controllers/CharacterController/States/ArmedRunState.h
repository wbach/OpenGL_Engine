#pragma once
#include "CharacterControllerCommonDefs.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunState
    : public RotatingMoveState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedRunState(FsmContext& context);

    using RotatingMoveState::onEnter;
    using RotatingMoveState::update;

    Utils::StateMachine::Maybe<Utils::StateMachine::TransitionTo<ArmedAttackState>,
                               Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>,
                               Utils::StateMachine::TransitionTo<ArmedAttackAndRunState>>
    handle(const AttackEvent&);

    using Will::handle;

    void onMoveInactivity() override;
};
}  // namespace Components
}  // namespace GameEngine
