#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "DrawArrowStateBase.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilWalkState;
class ArmedWalkState;
class ArmedSprintState;
class DrawArrowState;
class DrawArrowWalkState;
class DrawArrowWalkAndRotateState;
class DisarmedWalkState;
class AimWalkState;
class DeathState;
class WalkArmedChangeState;

class DrawArrowWalkState
    : public DrawArrowStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<WalkArmedChangeState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<SprintStateChangeEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>>
{
public:
    DrawArrowWalkState(FsmContext&);
    void onEnter(const DrawArrowEvent&);
    void onEnter(const ReloadArrowEvent&);
    void onEnter(const WalkForwardEvent&);
    void onEnter(const WalkBackwardEvent&);
    void onEnter(const RunForwardEvent&);
    void onEnter(const RunBackwardEvent&);
    void onEnter(const WalkChangeStateEvent&);

    void update(const WalkForwardEvent&);
    void update(const WalkBackwardEvent&);
    void update(const RunForwardEvent&);
    void update(const RunBackwardEvent&);

    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStateChangeEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
