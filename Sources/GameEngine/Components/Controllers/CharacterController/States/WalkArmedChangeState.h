#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class JumpState;
class MoveJumpState;
class DeathState;
class ArmedRunState;
class ArmedWalkState;
class DrawArrowWalkState;
class DisarmedRunState;
class DisarmedWalkState;
class DisarmedIdleState;
class DisarmedSprintState;
class DisarmedWalkAndRotateState;
class RunArmedChangeState;
class IdleArmedChangeState;
class ArmedIdleState;
class WalkAndRotateArmedChangeState;
class RecoilWalkState;
class AimWalkState;

class WalkArmedChangeState
    : public ArmedChangeStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<RunArmedChangeState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,  // quque?
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>       // queue?
{
public:
    WalkArmedChangeState(FsmContext& context);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;
    using MoveStateBase::onEnter;
    using MoveStateBase::update;

    void onEnter();

    void onEnter(DisarmedWalkState&, const WeaponStateEvent&);
    void onEnter(ArmedWalkState&, const WeaponStateEvent&);
    void onEnter(DisarmedWalkState&, const DrawArrowEvent&);

    void onEnter(DrawArrowWalkState&, const WeaponStateEvent&);
    void onEnter(RecoilWalkState&, const WeaponStateEvent&);
    void onEnter(AimWalkState&, const WeaponStateEvent&);

    void update(float);
    void update(const WeaponStateEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
