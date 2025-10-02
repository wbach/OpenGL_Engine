#pragma once
#include "AimStateBase.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimCrouchWalkState
    : public AimStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<AimCrouchState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<AimCrouchState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<AimCrouchState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<AimCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<DodgeDiveEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeForwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeBackwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeLeftEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeRightEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkState>>>
{
public:
    AimCrouchWalkState(FsmContext&);

    void onEnter(const AimStartEvent&);

    using MoveStateBase::onEnter;
    using MoveStateBase::transitionCondition;
    using MoveStateBase::update;

    void update(float);

    void onLeave(const WeaponStateEvent&);
    void onLeave(const AimStopEvent&);
    void onLeave(const SprintStateChangeEvent&);

    void onMoveInactivity() override;

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
