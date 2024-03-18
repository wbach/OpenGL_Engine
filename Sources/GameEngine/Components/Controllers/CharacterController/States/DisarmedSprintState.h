#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class JumpState;
class MoveJumpState;
class DeathState;
class ArmedRunState;
class DisarmedRunState;
class DisarmedIdleState;
class DisarmedWalkState;
class DisarmedRunAndRotateState;
class DisarmedSprintAndRotateState;
class RunArmedChangeState;

class DisarmedSprintState
    : public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DisarmedSprintAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DisarmedSprintAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DisarmedSprintAndRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    DisarmedSprintState(FsmContext& context)
        : MoveStateBase{context, std::nullopt, context.sprintSpeed, context.animClipNames.disarmed.sprint}
    {
    }

    using MoveStateBase::onEnter;
};
}  // namespace Components
}  // namespace GameEngine
