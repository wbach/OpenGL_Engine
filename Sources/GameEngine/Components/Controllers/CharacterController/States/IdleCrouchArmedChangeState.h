#pragma once
#include "ArmedChangeStateBase.h"
#include "CharacterControllerCommonDefs.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class IdleCrouchArmedChangeState
    : public ArmedChangeStateBase,
      public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedIdleCrouchState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleCrouchState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<CrouchWalkArmedChangeState>>,
          Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<CrouchRotateArmedChangeState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>
{
public:
    IdleCrouchArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::update;
    using IdleStateBase::onEnter;

    void onEnter(const CrouchChangeStateEvent&);
    void onEnter(IdleArmedChangeState&, const CrouchChangeStateEvent&);
    void onEnter(DisarmedIdleCrouchState&, const WeaponStateEvent&);
    void onEnter(DisarmedIdleCrouchState&, const DrawArrowEvent&);
    void onEnter(ArmedIdleCrouchState&, const WeaponStateEvent&);

    void onEnter(AimCrouchState&, const WeaponStateEvent&);
    void onEnter(DrawArrowCrouchState&, const WeaponStateEvent&);
    void onEnter(RecoilCrouchState&, const WeaponStateEvent&);

    void update(float);

    using ArmedChangeStateBase::onLeave;

private:
    void setAnim();

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
