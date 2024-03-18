#pragma once
#include <Utils/Fsm/Actions.h>

#include "DrawArrowStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class ArmedIdleState;
class ArmedSprintState;
class DisarmedIdleState;
class DrawArrowWalkState;
class DrawArrowRotateState;
class IdleArmedChangeState;
class DeathState;

class DrawArrowState
    : public DrawArrowStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    DrawArrowState(FsmContext&);

    using DrawArrowStateBase::onEnter;
    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
};
}  // namespace Components
}  // namespace GameEngine
