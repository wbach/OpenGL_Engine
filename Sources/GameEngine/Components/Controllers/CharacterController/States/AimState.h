#pragma once
#include "AimStateBase.h"
#include "CharacterControllerCommonDefs.h"

namespace GameEngine
{
namespace Components
{
class AimState : public AimStateBase,
                 public Utils::StateMachine::Will<
                     Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                     Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                     Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<MoveLeftEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<MoveRightEvent, Utils::StateMachine::TransitionTo<AimWalkState>>,
                     Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleArmedChangeState>>,
                     Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<AimRotateState>>,
                     Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<RecoilState>>,
                     Utils::StateMachine::On<CrouchChangeStateEvent, Utils::StateMachine::TransitionTo<AimCrouchState>>,
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
    void onEnter(const CrouchChangeStateEvent&);

    void onLeave(const AimStopEvent&);
    void onLeave(const WeaponStateEvent&);
};
}  // namespace Components
}  // namespace GameEngine
