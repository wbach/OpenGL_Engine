#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "AimStateBase.h"
#include "MoveStateBase.h"
#include "RotateStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilRunState;
class RecoilRunAndRotateState;
class ArmedRunAndRotateState;
class AimRunAndRotateState;
class AimWalkAndRotateState;
class DisarmedRunAndRotateState;

class AimRunAndRotateState
    : public AimStateBase,
      public MoveStateBase,
      public RotateStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<AimWalkAndRotateState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunAndRotateState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilRunAndRotateState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedRunAndRotateState>>>
{
public:
    AimRunAndRotateState(FsmContext&);
    void onEnter();
    void onEnter(const AimStartEvent&);
    void onEnter(const RunForwardEvent&);
    void onEnter(const RunBackwardEvent&);
    void onEnter(const RotateLeftEvent&);
    void onEnter(const RotateRightEvent&);

    void update(float);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);

private:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
