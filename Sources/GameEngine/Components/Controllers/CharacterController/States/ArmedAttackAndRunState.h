#pragma once
#include "AttackStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class ArmedAttackAndRunState
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
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::Queue>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::RemoveFromQueue<DrawArrowEvent>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedAttackState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedAttackState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedAttackState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<ArmedAttackState>>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedAttackAndRunAndRotateState>>>
{
public:
    ArmedAttackAndRunState(FsmContext&);

    using AttackStateBase::onEnter;
    using AttackStateBase::onLeave;
    using AttackStateBase::update;

    using RotatingMoveState::onEnter;
    using RotatingMoveState::update;

    using MaybeAttackStates =
        Utils::StateMachine::Maybe<Utils::StateMachine::Update, Utils::StateMachine::TransitionTo<ArmedAttackState>,
                                   Utils::StateMachine::TransitionTo<ArmedAttackAndWalkState>>;

    MaybeAttackStates handle(const ChangeAnimEvent&);

    using Will::handle;

    void update(float);

    void onMoveInactivity() override;

private:
    FsmContext& context;
};
}  // namespace Components
}  // namespace GameEngine
