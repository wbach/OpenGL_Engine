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
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
//          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
//          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
//          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<ReloadArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<RecoilState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleCrouchState>>>
{
public:
    RecoilCrouchState(FsmContext &);

    using IdleStateBase::onEnter;
    using RecoilStateBase::onEnter;

    void onEnter(const EndRotationEvent &);
    void onEnter(const EndForwardMoveEvent &);
    void onEnter(const EndBackwardMoveEvent &);

    void onLeave(const AimStopEvent &);
    void onLeave(const WeaponStateEvent &);

    void update(float);

private:
    FsmContext &context_;
};
}  // namespace Components
}  // namespace GameEngine
