#pragma once
#include "CharacterControllerCommonDefs.h"
#include "DrawArrowStateBase.h"
#include "MoveAndRotateStateBase.h"


namespace GameEngine
{
namespace Components
{
class DrawArrowWalkAndRotateState
    : public DrawArrowStateBase,
      public MoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintAndRotateState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowRotateState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimWalkAndRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>>
{
public:
    DrawArrowWalkAndRotateState(FsmContext&);

    using MoveAndRotateStateBase::onEnter;
    using MoveAndRotateStateBase::transitionCondition;
    using MoveAndRotateStateBase::update;

    void onEnter(ArmedSprintAndRotateState&, const DrawArrowEvent&);
    void onEnter(ArmedRunAndRotateState&, const DrawArrowEvent&);
    void onEnter(ArmedWalkAndRotateState&, const DrawArrowEvent&);

    void onEnter(const DrawArrowEvent&);
    void onEnter(const ReloadArrowEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);
    void onEnter(const RotateTargetEvent&);
    void onEnter(const MoveForwardEvent&);
    void onEnter(const MoveBackwardEvent&);
    void onEnter(const WalkChangeStateEvent&);

    void update(float);
    void update(const RotateLeftEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateTargetEvent&);
    void update(const MoveForwardEvent&);
    void update(const MoveBackwardEvent&);
    void update(const MoveLeftEvent&);
    void update(const MoveRightEvent&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStateChangeEvent&);

    void onMoveInactivity() override;

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
