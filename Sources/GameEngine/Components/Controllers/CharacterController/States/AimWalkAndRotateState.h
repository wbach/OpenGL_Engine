#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "AimStateBase.h"
#include "MoveStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilWalkState;
class RecoilWalkAndRotateState;
class ArmedWalkAndRotateState;
class AimWalkAndRotateState;
class AimRunAndRotateState;
class DisarmedWalkAndRotateState;

class AimWalkAndRotateState
    : public AimStateBase,
      public MoveStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<AimRunAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkAndRotateState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilWalkAndRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>>
{
public:
    AimWalkAndRotateState(FsmContext&);
    void update(float);
};
}  // namespace Components
}  // namespace GameEngine
