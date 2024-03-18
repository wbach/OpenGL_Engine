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
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::Update>,
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
    void onEnter(const MoveForwardEvent&);
    void onEnter(const MoveBackwardEvent&);
    void onEnter(const MoveLeftEvent&);
    void onEnter(const MoveRightEvent&);
    void onEnter(const WalkChangeStateEvent&);

    void update(float);
    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateTargetEvent&);
    void update(const MoveForwardEvent&);
    void update(const MoveBackwardEvent&);
    void update(const MoveLeftEvent&);
    void update(const MoveRightEvent&);

    void onMoveInactivity() override;

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStateChangeEvent&);
};
}  // namespace Components
}  // namespace GameEngine
