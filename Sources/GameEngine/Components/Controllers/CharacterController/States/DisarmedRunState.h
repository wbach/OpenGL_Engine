#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"

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
    : public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
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
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<DisarmedSprintState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    DisarmedRunState(FsmContext& context);

    using MoveStateBase::onEnter;
};
}  // namespace Components
}  // namespace GameEngine
