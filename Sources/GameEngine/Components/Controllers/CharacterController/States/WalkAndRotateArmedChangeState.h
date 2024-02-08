#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "ArmedChangeStateBase.h"
#include "MoveAndRotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class AimWalkAndRotateState;
class ArmedWalkAndRotateState;
class DrawArrowWalkState;
class DrawArrowRotateState;
class DrawArrowRunAndRotateState;
class DisarmedWalkAndRotateState;
class DeathState;
class ArmedSprintAndRotateState;
class RunAndRotateArmedChangeState;
class RotateArmedChangeState;
class WalkArmedChangeState;
class DisarmedRunAndRotateState;
class ArmedRunAndRotateState;
class RecoilWalkAndRotateState;
class DrawArrowWalkAndRotateState;
class DisarmedSprintAndRotateState;
class ArmedSprintAndRotateState;

class WalkAndRotateArmedChangeState
    : public ArmedChangeStateBase,
      public MoveAndRotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<EquipEndStateEvent, Utils::StateMachine::TransitionTo<ArmedWalkAndRotateState>>,
          Utils::StateMachine::On<DisarmEndStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkAndRotateState>>,

          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,

          Utils::StateMachine::On<SprintStateChangeEvent,
                                  Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<WalkChangeStateEvent,
                                  Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<RunAndRotateArmedChangeState>>,
          Utils::StateMachine::On<EndRotationEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<RotateArmedChangeState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::Update>,  // quque?
          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::Update>>       // queue?
{
public:
    WalkAndRotateArmedChangeState(FsmContext&);

    using ArmedChangeStateBase::update;
    using ArmedChangeStateBase::onLeave;
    
    using MoveAndRotateStateBase::update;
    using MoveAndRotateStateBase::onEnter;
    using MoveAndRotateStateBase::transitionCondition;

    void onEnter(DisarmedWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(ArmedWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(DisarmedWalkAndRotateState&, const DrawArrowEvent&);

    void onEnter(DrawArrowWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(RecoilWalkAndRotateState&, const WeaponStateEvent&);
    void onEnter(AimWalkAndRotateState&, const WeaponStateEvent&);

    void update(float);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
