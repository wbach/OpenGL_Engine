#pragma once
#include <Utils/Fsm/Actions.h>

#include "../CharacterControllerEvents.h"
#include "../FsmContext.h"
#include "AimStateBase.h"

namespace GameEngine
{
namespace Components
{
class RecoilState;
class ArmedIdleState;
class AimRunState;
class AimWalkState;
class AimRotateState;
class DisarmedIdleState;

class AimState : public AimStateBase,
                 public Utils::StateMachine::Will<
                     Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                     Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<AimRunState>>,
                     Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<AimRunState>>,
                     Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<DisarmedIdleState>>,
                     Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilState>>,
                     Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    AimState(FsmContext&);
};
}  // namespace Components
}  // namespace GameEngine
