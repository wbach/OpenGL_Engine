#pragma once
#include "AimStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimCrouchState
    : public AimStateBase,
      public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<AimCrouchWalkState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimCrouchRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimCrouchRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<AimCrouchRotateState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<AimState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<DodgeDiveEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeForwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeBackwardEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeLeftEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<DodgeRightEvent, Utils::StateMachine::TransitionTo<DodgeState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleCrouchState>>>
{
public:
    AimCrouchState(FsmContext &);

    using AimStateBase::onEnter;
    using IdleStateBase::onEnter;

    void onEnter(const EndRotationEvent &);
    void onEnter(const EndForwardMoveEvent &);
    void onEnter(const EndBackwardMoveEvent &);
    void onEnter(const EndMoveLeftEvent &);
    void onEnter(const EndMoveRightEvent &);

    void onLeave(const AimStopEvent &);
    void onLeave(const WeaponStateEvent &);

    void update(float);

private:
    FsmContext &context_;
};
}  // namespace Components
}  // namespace GameEngine
