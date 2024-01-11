#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class JumpState;
class DeathState;
class ArmedIdleState;
class JointPoseUpdater;

class IdleState : public Utils::StateMachine::Will<
                      Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                      // Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
                      Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
                      Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<ArmedIdleState>>,
                      Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
                      Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                      Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                      Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                      Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                      Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                      Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    IdleState(FsmContext&,  float disarmTimeStamp);
    void onEnter();
    void onEnter(const WeaponStateEvent&);
    void update(const AttackEvent&);
    void update(const EndAttackEvent&);
    void update(float);
    void onLeave();

private:
    void unsubscribe();

private:
    FsmContext& context_;
    std::string idleAnimName_;
    std::string disarmAnimName_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
    bool weaponChangeTriggered_{false};
    float disarmTimeStamp_{0.0};

    JointPoseUpdater* jointPoseUpdater_;
};
}  // namespace Components
}  // namespace GameEngine
