#pragma once
#include <optional>

#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
class RunArmedChangeState
    : public ArmedChangeStateBase,
      public RotatingMoveState,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<ArmedFallingState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue?
{
public:
    RunArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;
    using RotatingMoveState::onEnter;
    using RotatingMoveState::update;

    void onEnter();

    void onEnter(const SprintStateChangeEvent&);

    void onEnter(DisarmedRunState&, const WeaponStateEvent&);
    void onEnter(ArmedRunState&, const WeaponStateEvent&);
    void onEnter(DisarmedRunState&, const DrawArrowEvent&);

    void onEnter(DisarmedSprintState&, const DrawArrowEvent&);
    void onEnter(DisarmedSprintState&, const WeaponStateEvent&);
    void onEnter(ArmedSprintState&, const WeaponStateEvent&);

    void onEnter(DrawArrowWalkState&, const WeaponStateEvent&);
    void onEnter(RecoilWalkState&, const WeaponStateEvent&);
    void onEnter(AimWalkState&, const WeaponStateEvent&);

    void update(float);
    void update(const SprintStateChangeEvent&);
    void update(const WeaponStateEvent&);

    void onMoveInactivity() override;

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
