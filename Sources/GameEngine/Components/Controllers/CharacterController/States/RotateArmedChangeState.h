#pragma once
#include <Logger/Log.h>
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class DisarmedIdleState;
class DisarmedRotateState;
class DisarmedRunAndRotateState;
class DisarmedWalkAndRotateState;
class DisarmedSprintAndRotateState;
class DrawArrowRotateState;
class JumpState;
class DeathState;
class ArmedRotateState;
class ArmedSprintAndRotateState;
class IdleArmedChangeState;
class RunAndRotateArmedChangeState;
class WalkAndRotateArmedChangeState;
class ArmedRunAndRotateState;

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
          //Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<ArmedRotateState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<WalkAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
          Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>
{
public:
    RotateArmedChangeState(FsmContext&);

    using RotateStateBase::onEnter;
    using RotateStateBase::update;

    void onEnter();
    void onEnter(DisarmedRotateState&, const WeaponStateEvent&);
    void onEnter(ArmedRotateState&, const WeaponStateEvent&);
    void onEnter(DisarmedRotateState&, const DrawArrowEvent&);

    void update(float);
    void update(const DrawArrowEvent&);
    void update(const AimStopEvent&);
    void update(const RotateRightEvent&);
    void update(const RotateLeftEvent&);
    void update(const RotateTargetEvent&);

    using ArmedChangeStateBase::onLeave;
    void onLeave(const EquipEndStateEvent&);

private:
    FsmContext& context_;
    bool drawArrowEventCalled_{false};
};
}  // namespace Components
}  // namespace GameEngine
