#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotatingMoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
struct FsmContext;
class CrouchWalkAndRotateArmedChangeState
    : public ArmedChangeStateBase,
      public RotatingMoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedCrouchWalkAndRotateState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
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
    CrouchWalkAndRotateArmedChangeState(FsmContext &);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;
    using RotatingMoveAndRotateStateBase::onEnter;
    using RotatingMoveAndRotateStateBase::transitionCondition;
    using RotatingMoveAndRotateStateBase::update;

    void onEnter();
    void onEnter(const SprintStateChangeEvent &);

    void onEnter(WalkAndRotateArmedChangeState &, const CrouchChangeStateEvent &);
    void onEnter(DisarmedCrouchWalkAndRotateState &, const WeaponStateEvent &);
    void onEnter(ArmedCrouchWalkAndRotateState &, const WeaponStateEvent &);

    void onEnter(DisarmedCrouchWalkAndRotateState &, const DrawArrowEvent &);

    void onEnter(ArmedSprintAndRotateState &, const WeaponStateEvent &);
    void onEnter(DisarmedSprintAndRotateState &, const DrawArrowEvent &);
    void onEnter(DisarmedSprintAndRotateState &, const WeaponStateEvent &);

    void onEnter(DrawArrowCrouchWalkAndRotateState &, const WeaponStateEvent &);
    void onEnter(RecoilCrouchWalkAndRotateState &, const WeaponStateEvent &);
    void onEnter(AimCrouchWalkAndRotateState &, const WeaponStateEvent &);

    void update(float);
    void update(const WeaponStateEvent &);

private:
    FsmContext &context_;
    bool drawArrowEventCalled_{false};
    bool sprintEventCalled_{false};
};
}  // namespace Components
}  // namespace GameEngine
