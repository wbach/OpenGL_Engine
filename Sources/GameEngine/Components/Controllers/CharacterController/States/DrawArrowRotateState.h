#pragma once
#include <Utils/Fsm/Actions.h>

#include "DrawArrowStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class AimRotateState;
class ArmedRotateState;
class ArmedSprintAndRotateState;
class DrawArrowState;
class DisarmedRotateState;
class DrawArrowWalkAndRotateState;
class RotateArmedChangeState;
class DeathState;

class DrawArrowRotateState
    : public DrawArrowStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>>
{
public:
    DrawArrowRotateState(FsmContext&);
    void onEnter(const DrawArrowEvent&);
    void onEnter(const ReloadArrowEvent&);
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
#pragma once
