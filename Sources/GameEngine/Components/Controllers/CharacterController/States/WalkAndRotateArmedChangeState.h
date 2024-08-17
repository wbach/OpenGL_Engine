#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotatingMoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class WalkAndRotateArmedChangeState
    : public ArmedChangeStateBase,
      public RotatingMoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkAndRotateState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<EndMoveLeftEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<EndMoveRightEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<CrouchWalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>
{
public:
    WalkAndRotateArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;

    void onEnter();

    using RotatingMoveAndRotateStateBase::onEnter;
    using RotatingMoveAndRotateStateBase::transitionCondition;
    using RotatingMoveAndRotateStateBase::update;

    void onEnter(DisarmedCrouchWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(DisarmedWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(ArmedWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(DisarmedWalkAndRotateState&, const DrawArrowEvent&);

    void onEnter(DrawArrowWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(RecoilWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(AimWalkAndRotateState&, const WeaponStateEvent&);

    void update(float);
    void update(const WeaponStateEvent&);

private:
    void setEquipAnimations();
    void setDisarmAnimations();

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
