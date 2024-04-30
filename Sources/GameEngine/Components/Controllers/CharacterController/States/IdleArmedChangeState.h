#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "IdleStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedIdleState;
class DisarmedRunState;
class DisarmedRotateState;
class DisarmedWalkState;
class DisarmedSprintState;
class DisarmedCrouchState;

class RunArmedChangeState;
class RunAndRotateArmedChangeState;
class WalkArmedChangeState;
class WalkAndRotateArmedChangeState;
class RotateArmedChangeState;

class DrawArrowState;
class ArmedIdleState;
class AimState;
class RecoilState;
class JumpState;
class DeathState;

class IdleArmedChangeState
    : public ArmedChangeStateBase,
      public IdleStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          /* Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
           Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,*/
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue?
{
public:
    IdleArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::update;
    using IdleStateBase::onEnter;

    void onEnter(DisarmedIdleState&, const WeaponStateEvent&);
    void onEnter(ArmedIdleState&, const WeaponStateEvent&);
    void onEnter(DisarmedIdleState&, const DrawArrowEvent&);

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
