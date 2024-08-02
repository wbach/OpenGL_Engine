#pragma once
#include "CharacterControllerCommonDefs.h"
#include "DrawArrowStateBase.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class DrawArrowCrouchState
    : public DrawArrowStateBase,
      public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchWalkState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchWalkState>>,
          //          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          //          Utils::StateMachine::On<RotateRightEvent,
          //          Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          //          Utils::StateMachine::On<RotateTargetEvent,
          //          Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimCrouchState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleCrouchState>>>
{
public:
    DrawArrowCrouchState(FsmContext &);

    using DrawArrowStateBase::onEnter;
    void onEnter();
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
