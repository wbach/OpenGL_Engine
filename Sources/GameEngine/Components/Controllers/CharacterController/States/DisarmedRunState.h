#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class DeathState;
class JumpState;
class MoveJumpState;
class ArmedRunState;
class DisarmedIdleState;
class DisarmedWalkState;
class DisarmedSprintState;
class DisarmedRunAndRotateState;
class RunArmedChangeState;

class DisarmedRunState
    : public RotatingMoveState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<DisarmedSprintState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    DisarmedRunState(FsmContext& context);

    using RotatingMoveState::onEnter;
    using RotatingMoveState::update;

    void onMoveInactivity() override;
};
}  // namespace Components
}  // namespace GameEngine
