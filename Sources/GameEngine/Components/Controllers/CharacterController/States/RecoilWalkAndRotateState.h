#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"
#include "RecoilStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimWalkAndRotateState;
class ArmedWalkAndRotateState;
class ArmedSprintAndRotateState;
class RecoilWalkState;
class RecoilRotateState;
class DisarmedWalkAndRotateState;
class DrawArrowWalkAndRotateState;
class WalkAndRotateArmedChangeState;
class DeathState;

class RecoilWalkAndRotateState
    : public RecoilStateBase,
      public MoveStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveInactivityEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateInactivityEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<RecoilRotateState>>,
          Utils::StateMachine::On<ReloadArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>>
{
public:
    RecoilWalkAndRotateState(FsmContext&);

    using MoveStateBase::onEnter;
    using MoveStateBase::transitionCondition;
    using MoveStateBase::update;

    void onEnter(const AttackEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void onEnter(const WalkForwardEvent&);
    void onEnter(const WalkBackwardEvent&);
    void onEnter(const RunForwardEvent&);
    void onEnter(const RunBackwardEvent&);
    void onEnter(const WalkChangeStateEvent&);

    void update(float);
    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateTargetEvent&);
    void update(const WalkForwardEvent&);
    void update(const WalkBackwardEvent&);
    void update(const RunForwardEvent&);
    void update(const RunBackwardEvent&);
    void update(const MoveInactivityEvent&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStateChangeEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
