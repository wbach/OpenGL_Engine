#pragma once
#include <Utils/Fsm/Actions.h>

#include <optional>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class DeathState;
class JumpState;
class MoveJumpState;
class ArmedRunState;
class ArmedWalkState;
class ArmedSprintState;
class DisarmedRunState;
class ArmedIdleState;
class DisarmedIdleState;
class DisarmedWalkState;
class DisarmedSprintState;
class DisarmedRunAndRotateState;

class IdleArmedChangeState;
class WalkArmedChangeState;
class RunAndRotateArmedChangeState;
class DrawArrowRunState;
class RecoilRunState;
class AimRunState;

class RunArmedChangeState
    : public ArmedChangeStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          // Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>  // queue?
{
public:
    RunArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::onLeave;
    using ArmedChangeStateBase::update;
    using MoveStateBase::onEnter;
    using MoveStateBase::update;

    void onEnter();

    void onEnter(const SprintStartEvent&);
    void onEnter(const SprintStateChangeEvent&);

    void onEnter(DisarmedRunState&, const WeaponStateEvent&);
    void onEnter(ArmedRunState&, const WeaponStateEvent&);
    void onEnter(DisarmedRunState&, const DrawArrowEvent&);

    void onEnter(DisarmedSprintState&, const DrawArrowEvent&);
    void onEnter(DisarmedSprintState&, const WeaponStateEvent&);
    void onEnter(ArmedSprintState&, const WeaponStateEvent&);

    void onEnter(DrawArrowRunState&, const WeaponStateEvent&);
    void onEnter(RecoilRunState&, const WeaponStateEvent&);
    void onEnter(AimRunState&, const WeaponStateEvent&);

    void update(float);
    void update(const SprintStateChangeEvent&);
    void update(const WeaponStateEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
