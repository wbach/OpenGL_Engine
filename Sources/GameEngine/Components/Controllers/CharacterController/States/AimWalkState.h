#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "AimStateBase.h"
#include "MoveStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilWalkState;
class ArmedWalkState;
class AimState;
class AimRunState;
class AimWalkAndRotateState;
class DisarmedWalkState;

class AimWalkState
    : public AimStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<AimRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedWalkState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimWalkAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimWalkAndRotateState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilWalkState>>,
          Utils::StateMachine::On<EndForwardMoveEvent, Utils::StateMachine::TransitionTo<AimState>>,
          Utils::StateMachine::On<EndBackwardMoveEvent, Utils::StateMachine::TransitionTo<AimState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedWalkState>>>
{
public:
    AimWalkState(FsmContext&);
    using AimStateBase::onEnter;
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
