#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"

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

class ArmedWalkState
    : public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponChangeEndEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<MoveJumpState>>>
{
public:
    ArmedWalkState(FsmContext& context)
        : MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                        context.animClipNames.armed.walk.backward}
    {
    }

    using MoveStateBase::onEnter;

    void onEnter(const WeaponStateEvent&)
    {
        context_.multiAnimations = true;
        StateBase::equipWeapon();
        MoveStateBase::setCurrentAnim();
    }
};
}  // namespace Components
}  // namespace GameEngine
