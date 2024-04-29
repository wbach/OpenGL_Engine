#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "RotatingMoveAndRotateStateBase.h"

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
class ArmedRotateState;
class ArmedRunAndRotateState;
class DisarmedWalkAndRotateState;
class ArmedSprintAndRotateState;
class WalkAndRotateArmedChangeState;
class RotateArmedChangeState;
class RunArmedChangeState;
class RecoilWalkAndRotateState;
class AimWalkAndRotateState;
class DrawArrowWalkAndRotateState;

struct FsmContext;
class RunAndRotateArmedChangeState
    : public ArmedChangeStateBase,
      public RotatingMoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<WalkChangeStateEvent,
                                  Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue,
{
public:
    RunAndRotateArmedChangeState(FsmContext &context);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;
    using RotatingMoveAndRotateStateBase::onEnter;
    using RotatingMoveAndRotateStateBase::transitionCondition;
    using RotatingMoveAndRotateStateBase::update;

    void onEnter();
    void onEnter(const SprintStateChangeEvent &);

    void onEnter(ArmedSprintAndRotateState &, const WeaponStateEvent &);
    void onEnter(DisarmedSprintAndRotateState &, const DrawArrowEvent &);
    void onEnter(DisarmedSprintAndRotateState &, const WeaponStateEvent &);

    void onEnter(DisarmedRunAndRotateState &, const DrawArrowEvent &);
    void onEnter(DisarmedRunAndRotateState &, const WeaponStateEvent &);
    void onEnter(ArmedRunAndRotateState &, const WeaponStateEvent &);

    void onEnter(DrawArrowWalkAndRotateState &, const WeaponStateEvent &);
    void onEnter(RecoilWalkAndRotateState &, const WeaponStateEvent &);
    void onEnter(AimWalkAndRotateState &, const WeaponStateEvent &);

    void update(float);
    void update(const WeaponStateEvent &);

private:
    FsmContext &context_;
    bool drawArrowEventCalled_{false};
    bool sprintEventCalled_{false};
};
}  // namespace Components
}  // namespace GameEngine
