#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunState;
class ArmedIdleState;
class ArmedSprintState;
class ArmedWalkAndRotateState;
class JumpState;
class MoveJumpState;
class DeathState;
class DisarmedWalkState;
class DrawArrowWalkState;
class WalkArmedChangeState;

class ArmedWalkState
    : public RotatingMoveState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    ArmedWalkState(FsmContext& context)
        : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.armed.walk.forward}
    {
    }

    using MoveStateBase::onEnter;

    void onMoveInactivity() override
    {
        setAnim(context_.animClipNames.armed.idle);
    }
};
}  // namespace Components
}  // namespace GameEngine
