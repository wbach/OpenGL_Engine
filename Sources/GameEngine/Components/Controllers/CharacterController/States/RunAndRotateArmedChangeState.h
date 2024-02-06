#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "MoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedWalkAndRotateState;
class DisarmedSprintAndRotateState;
class DisarmedRunAndRotateState;

class JumpState;
class DeathState;
class ArmedRunAndRotateState;
class DrawArrowRunAndRotateState;
class WalkAndRotateArmedChangeState;
class RotateArmedChangeState;
class RunArmedChangeState;

struct FsmContext;
class RunAndRotateArmedChangeState
    : public ArmedChangeStateBase,
      public MoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<WalkChangeStateEvent,
                                  Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,  // queue
          Utils::StateMachine::On<SprintStateChangeEvent,
                                  Utils::StateMachine::Update>,             // queue,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue,
{
public:
    RunAndRotateArmedChangeState(FsmContext &context);

    using MoveAndRotateStateBase::onEnter;
    using MoveAndRotateStateBase::transitionCondition;
    using MoveAndRotateStateBase::update;

    void onEnter(const WeaponStateEvent &);
    void update(float);

private:
    FsmContext &context_;
};
}  // namespace Components
}  // namespace GameEngine
