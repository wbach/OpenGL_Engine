#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class ArmedWalkState;
class ArmedRotateState;
class ArmedRunAndRotateState;
class ArmedSprintAndRotateState;
class DisarmedWalkAndRotateState;
class DrawArrowWalkAndRotateState;
class WalkAndRotateArmedChangeState;
class JumpState;
class DeathState;

struct FsmContext;

class ArmedWalkAndRotateState
    : public MoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedWalkAndRotateState(FsmContext &);

    using MoveAndRotateStateBase::onEnter;
    using MoveAndRotateStateBase::transitionCondition;
    using MoveAndRotateStateBase::update;
};
}  // namespace Components
}  // namespace GameEngine
