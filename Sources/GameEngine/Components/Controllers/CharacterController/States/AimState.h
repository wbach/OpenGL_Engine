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
class AimWalkState;
class AimRotateState;
class DisarmedIdleState;
class DeathState;
class ArmedSprintState;
class IdleArmedChangeState;

class AimState : public AimStateBase,
                 public Utils::StateMachine::Will<
                     Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                     Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                     Utils::StateMachine::On<WalkForwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<WalkBackwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<RunForwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<RunBackwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<RunLeftEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<RunRightEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<WalkLeftEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<WalkRightEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<SprintStartEvent, Utils::StateMachine::TransitionTo<ArmedSprintState>>,
                     Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
                     Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilState>>,
                     Utils::StateMachine::On<AimStopEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>>
{
public:
    AimState(FsmContext&);
    using AimStateBase::onEnter;
    void onEnter(const EndRotationEvent&);
    void onEnter(const EndForwardMoveEvent&);
    void onEnter(const EndBackwardMoveEvent&);
    void onEnter(const EndMoveLeftEvent&);
    void onEnter(const EndMoveRightEvent&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
    void onLeave(const SprintStartEvent&);
};
}  // namespace Components
}  // namespace GameEngine
