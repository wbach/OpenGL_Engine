#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunAndRotateState;
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedSprintState;
class DisarmedWalkAndRotateState;
class DisarmedRunAndRotateState;
class RunAndRotateArmedChangeState;
class WalkAndRotateArmedChangeState;
class JumpState;
class DeathState;
struct FsmContext;

class DisarmedSprintAndRotateState
    : public MoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent,
                                  Utils::StateMachine::TransitionTo<DisarmedWalkAndRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DisarmedSprintState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    DisarmedSprintAndRotateState(FsmContext &context)
        : MoveAndRotateStateBase{context,
                                 std::nullopt,
                                 context.sprintSpeed,
                                 context.animClipNames.disarmed.sprint,
                                 context.runSpeed.rotate,
                                 context.animClipNames.disarmed.rotateLeft,
                                 context.animClipNames.disarmed.rotateRight}
    {
    }

    using MoveAndRotateStateBase::onEnter;
    using MoveAndRotateStateBase::transitionCondition;
    using MoveAndRotateStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
