#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class CrouchRotateArmedChangeState
    : public ArmedChangeStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedCrouchRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedCrouchRotateState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<ArmedCrouchRotateState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<CrouchWalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<CrouchWalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<CrouchWalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<CrouchWalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>
{
public:
    CrouchRotateArmedChangeState(FsmContext&);

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
