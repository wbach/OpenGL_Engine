#pragma once
#include "CharacterControllerCommonDefs.h"
#include "RecoilStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilCrouchRotateState
    : public RecoilStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<RecoilCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<RecoilCrouchState>>,
          Utils::StateMachine::On<ReloadArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchRotateState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedCrouchRotateState>>>
{
public:
    RecoilCrouchRotateState(FsmContext&);

    using RecoilStateBase::onEnter;

    void onEnter(const CrouchChangeStateEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);

    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateTargetEvent&);
    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
};
}  // namespace Components
}  // namespace GameEngine
