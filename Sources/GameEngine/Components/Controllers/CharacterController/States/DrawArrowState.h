#pragma once
#include <Utils/Fsm/Actions.h>

#include "DrawArrowStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimState;
class ArmedIdleState;
class DisarmedIdleState;
class DrawArrowRunState;
class DrawArrowWalkState;
class DrawArrowRotateState;

class DrawArrowState
    : public DrawArrowStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowRunState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowRunState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    DrawArrowState(FsmContext&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
};
}  // namespace Components
}  // namespace GameEngine
