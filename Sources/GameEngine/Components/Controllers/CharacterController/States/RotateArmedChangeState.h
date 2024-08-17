#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotateStateBase.h"


namespace GameEngine
{
namespace Components
{
class RotateArmedChangeState
    : public ArmedChangeStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedRotateState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>
{
public:
    RotateArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::update;
    using RotateStateBase::onEnter;
    using RotateStateBase::update;

    void onEnter();

    void onEnter(DisarmedRotateState&, const WeaponStateEvent&);
    void onEnter(ArmedRotateState&, const WeaponStateEvent&);
    void onEnter(DisarmedRotateState&, const DrawArrowEvent&);

    void onEnter(DrawArrowRotateState&, const WeaponStateEvent&);
    void onEnter(RecoilRotateState&, const WeaponStateEvent&);
    void onEnter(AimRotateState&, const WeaponStateEvent&);

    void update(float);
    void update(const RotateRightEvent&);
    void update(const RotateLeftEvent&);
    void update(const RotateTargetEvent&);

    using ArmedChangeStateBase::onLeave;

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
