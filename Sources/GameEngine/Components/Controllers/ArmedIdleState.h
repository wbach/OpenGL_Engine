#pragma once
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
class ArmedMoveState;
class RotateState;
class JumpState;
class DeathState;
class IdleState;
class JointPoseUpdater;

class ArmedIdleState
    : public Utils::StateMachine::Will<
          Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
          //          Utils::StateMachine::On<AttackEvent, Utils::StateMachine::Update>,
          //          Utils::StateMachine::On<EndAttackEvent, Utils::StateMachine::Update>,
          Utils::StateMachine::On<WeaponStateEvent, Utils::StateMachine::TransitionTo<IdleState>>,
          Utils::StateMachine::On<DeathEvent, Utils::StateMachine::TransitionTo<DeathState>>,
          Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<ArmedMoveState>>,
          Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<ArmedMoveState>>,
          Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
          Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>>
//          Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
public:
    ArmedIdleState(FsmContext&, float);
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
    const std::string& idleAnimName_;
    const std::string& equipAnimName_;
    std::optional<uint32> subscribeForTransitionAnimationEnd_;
    float equipTimeStamp_{0.0f};

    JointPoseUpdater* jointPoseUpdater_;
};
}  // namespace Components
}  // namespace GameEngine
