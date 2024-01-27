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
class RecoilRunState;
class ArmedRunState;
class AimRunState;
class AimRunAndRotateState;
class DisarmedRunState;

class AimRunState
    : public AimStateBase,
      public MoveStateBase,
      public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WalkChangeStateEvent, Utils::StateMachine::TransitionTo<AimRunState>>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedRunState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimRunAndRotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimRunAndRotateState>>,
          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilRunState>>,
          Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedRunState>>>
{
public:
    AimRunState(FsmContext&);

    void onEnter();
    void onEnter(const AimStartEvent&);
    void onEnter(const RunForwardEvent&);
    void onEnter(const RunBackwardEvent&);

    void onLeave(const WeaponStateEvent&);
    void onLeave(const AimStopEvent&);

    void update(float);

protected:
    FsmContext& context_;
};
}  // namespace Components
}  // namespace GameEngine
