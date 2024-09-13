#pragma once
#include "CharacterControllerCommonDefs.h"
#include "IdleStateBase.h"
#include "RecoilStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilCrouchState
    : public RecoilStateBase,
      public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RecoilCrouchRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RecoilCrouchRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RecoilCrouchRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<ReloadArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<RecoilState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleCrouchState>>>
{
public:
    RecoilCrouchState(FsmContext &);

    using IdleStateBase::onEnter;
    using RecoilStateBase::onEnter;

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
