#pragma once
#include <Utils/FSM/Actions.h>

#include "CharacterControllerEvents.h"
#include "GameEngine/Components/Animation/Animator.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class JumpState;
class AttackState;

struct IdleState : public Utils::StateMachine::Will<
                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                       Utils::StateMachine::On<MoveForwardEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                       Utils::StateMachine::On<MoveBackwardEvent, Utils::StateMachine::TransitionTo<MoveState>>,
                       Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                       Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                       Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                       Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
// Utils::StateMachine::On<AttackEvent, Utils::StateMachine::TransitionTo<AttackState>>
{
    IdleState(Animator& animator, const std::string& animationName)
        : animator_{animator}
        , animationName_{animationName}
    {
    }

    void onEnter()
    {
        DEBUG_LOG("onEnter()");
        animator_.ChangeAnimation(animationName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                  std::nullopt);
    }

    void update(float)
    {
    }

    Animator& animator_;
    const std::string& animationName_;
};
}  // namespace Components
}  // namespace GameEngine
