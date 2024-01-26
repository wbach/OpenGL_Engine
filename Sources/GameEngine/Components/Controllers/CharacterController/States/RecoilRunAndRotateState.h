#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"
#include "RecoilStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimRunAndRotateState;
class ArmedRunAndRotateState;
class RecoilRunAndRotateState;
class RecoilWalkAndRotateState;
class DisarmedRunAndRotateState;
class DrawArrowRunAndRotateState;

class RecoilRunAndRotateState
    : public RecoilStateBase,
      public MoveStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<RecoilWalkAndRotateState>>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<RecoilWalkAndRotateState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<RecoilWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowRunAndRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>>
{
public:
    RecoilRunAndRotateState(FsmContext&);
    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
