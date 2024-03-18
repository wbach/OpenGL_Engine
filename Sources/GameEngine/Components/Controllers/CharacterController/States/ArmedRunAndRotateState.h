#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedRunState;
class ArmedRotateState;
class ArmedWalkAndRotateState;
class ArmedSprintAndRotateState;
class DisarmedRunAndRotateState;
class DrawArrowWalkAndRotateState;
class RunAndRotateArmedChangeState;
class JumpState;
class DeathState;

struct FsmContext;

class ArmedRunAndRotateState
    : public MoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedRunAndRotateState(FsmContext &context)
        : MoveAndRotateStateBase{context,
                                 std::nullopt,
                                 context.runSpeed,
                                 context.animClipNames.armed.run,
                                 context.animClipNames.armed.rotateLeft,
                                 context.animClipNames.armed.rotateRight}
    {
    }

    using MoveAndRotateStateBase::onEnter;
    using MoveAndRotateStateBase::transitionCondition;
    using MoveAndRotateStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
