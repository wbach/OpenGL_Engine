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
class DrawArrowState;
class DrawArrowRunState;
class DrawArrowWalkAndRotateState;
class DisarmedWalkState;
class AimWalkState;

class DrawArrowWalkState
    : public DrawArrowStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<DrawArrowRunState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<DrawArrowRunState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<DrawArrowRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkAndRotateState>>,
          Utils::StateMachine::On<AimStartEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<DrawArrowState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>>
{
public:
    DrawArrowWalkState(FsmContext&);
    void onEnter();
    void onEnter(const DrawArrowEvent&);
    void onEnter(const WalkForwardEvent&);
    void onEnter(const WalkBackwardEvent&);
    void onEnter(const WalkChangeStateEvent&);

    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
