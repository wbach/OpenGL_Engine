#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "MoveStateBase.h"
#include "RecoilStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilWalkState;
class ArmedWalkState;
class RecoilRunState;
class RecoilWalkAndRotateState;
class DisarmedWalkState;
class AimWalkState;
class DrawArrowWalkState;

class RecoilWalkState
    : public RecoilStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<RecoilRunState>>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<RecoilRunState>>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<RecoilRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RecoilWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RecoilWalkAndRotateState>>,
          Utils::StateMachine::On<DrawArrowEvent, Utils::StateMachine::TransitionTo<DrawArrowWalkState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>>
{
public:
    RecoilWalkState(FsmContext&);

    void onEnter();
    void onEnter(const AttackEvent&);
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
