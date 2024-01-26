#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "DrawArrowStateBase.h"
#include "MoveStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimWalkAndRotateState;
class ArmedWalkAndRotateState;
class DrawArrowRunAndRotateState;
class DisarmedWalkAndRotateState;

class DrawArrowWalkAndRotateState
    : public DrawArrowStateBase,
      public MoveStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowRunAndRotateState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowRunAndRotateState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowRunAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkAndRotateState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimWalkAndRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>>
{
public:
    DrawArrowWalkAndRotateState(FsmContext&);
    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
