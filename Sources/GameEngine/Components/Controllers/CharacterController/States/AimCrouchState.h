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
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
//          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
//          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
//          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<AimState>>,
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
