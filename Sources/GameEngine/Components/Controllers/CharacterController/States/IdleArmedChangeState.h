#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class IdleArmedChangeState
    : public ArmedChangeStateBase,
      public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<MoveEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<IdleCrouchArmedChangeState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue?
{
public:
    IdleArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::update;
    using IdleStateBase::onEnter;

    void onEnter(const CrouchChangeStateEvent&);
    void onEnter(IdleCrouchArmedChangeState&, const CrouchChangeStateEvent&);
    void onEnter(DisarmedIdleState&, const WeaponStateEvent&);
    void onEnter(DisarmedAttackState&, const WeaponStateEvent&);
    void onEnter(ArmedIdleState&, const WeaponStateEvent&);
    void onEnter(ArmedAttackState&, const WeaponStateEvent&);
    void onEnter(DisarmedIdleState&, const DrawArrowEvent&);
    void onEnter(DisarmedAttackState&, const DrawArrowEvent&);

    void onEnter(DrawArrowState&, const WeaponStateEvent&);
    void onEnter(RecoilState&, const WeaponStateEvent&);
    void onEnter(AimState&, const WeaponStateEvent&);

    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndRotationEvent&);

    void update(float);

    using ArmedChangeStateBase::onLeave;

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
