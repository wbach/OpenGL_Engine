#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "IdleStateBase.h"
#include "MoveStateBase.h"
#include "StateConcepts.h"
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
          Utils::StateMachine::On<HurtEvent, Utils::StateMachine::TransitionTo<HurtState>>,
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
          Utils::StateMachine::On<StartFallingEvent, Utils::StateMachine::TransitionTo<FallingState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue?
{
public:
    IdleArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;
    using IdleStateBase::onEnter;

    void onEnter(const CrouchChangeStateEvent&);
    void onEnter(IdleCrouchArmedChangeState&, const CrouchChangeStateEvent&);

    bool entryCondition(DisarmedIdleState&) const;
    bool entryCondition(DisarmedAttackState&) const;
    bool entryCondition(ArmedIdleState&) const;
    bool entryCondition(ArmedAttackState&) const;

    template <typename PrevState>
    requires IsArmed<PrevState>
    void onEnter(PrevState&, const StartDialogEvent& e)
    {
        ArmedChangeStateBase::disarmWeapon();
        pushEventToQueue(e);
    }

    void onEnter(DisarmedIdleState&, const WeaponStateEvent&);
    void onEnter(DisarmedAttackState&, const WeaponStateEvent&);
    void onEnter(DisarmedIdleState&, const DrawArrowEvent&);
    void onEnter(DisarmedIdleState&, const AimStartEvent&);
    void onEnter(DisarmedIdleState&, const ReloadArrowEvent&);
    void onEnter(DisarmedAttackState&, const DrawArrowEvent&);
    void onEnter(ArmedIdleState&, const WeaponStateEvent&);
    void onEnter(ArmedAttackState&, const WeaponStateEvent&);

    void onEnter(DrawArrowState&, const WeaponStateEvent&);
    void onEnter(RecoilState&, const WeaponStateEvent&);
    void onEnter(AimState&, const WeaponStateEvent&);

    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndRotationEvent&);

    void update(float);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
