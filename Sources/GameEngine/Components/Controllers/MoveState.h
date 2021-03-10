#pragma once
#include <Types.h>
#include <Utils/FSM/Actions.h>

#include "CharacterControllerEvents.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Rigidbody.h"

namespace GameEngine
{
namespace Components
{
class MoveState;
class RotateState;
class JumpState;
class AttackState;

struct MoveState : public Utils::StateMachine::Will<
                       Utils::StateMachine::ByDefault<Utils::StateMachine::Nothing>,
                       Utils::StateMachine::On<EndMoveEvent, Utils::StateMachine::TransitionTo<IdleState>>,
                       Utils::StateMachine::On<RotateLeftEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                       Utils::StateMachine::On<RotateRightEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                       Utils::StateMachine::On<RotateTargetEvent, Utils::StateMachine::TransitionTo<RotateState>>,
                       Utils::StateMachine::On<JumpEvent, Utils::StateMachine::TransitionTo<JumpState>>>
{
    MoveState(Rigidbody& rigidbody, Animator& animator, const std::string& forwardAnimationName,
              const std::string& backwardAnimationName)
        : rigidbody_{rigidbody}
        , animator_{animator}
        , forwardAnimationName_{forwardAnimationName}
        , backwardAnimationName_{backwardAnimationName}
        , direction_{0.f, 0.f, 1.f}
        , moveSpeed_{1.f}
    {
    }

    void onEnter(const MoveForwardEvent& event)
    {
        DEBUG_LOG("onEnter(const MoveForwardEvent&)");
        direction_ = event.direction;
        moveSpeed_ = fabsf(event.moveSpeed);
        DEBUG_LOG(std::to_string(moveSpeed_));
        DEBUG_LOG(std::to_string(direction_));
        if (not forwardAnimationName_.empty())
        {
            animator_.ChangeAnimation(forwardAnimationName_, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, std::nullopt);
        }
    }

    void onEnter(const MoveBackwardEvent& event)
    {
        DEBUG_LOG("onEnter(const MoveBackwardEvent&)");
        direction_ = event.direction;
        moveSpeed_ = fabsf(event.moveSpeed);
        DEBUG_LOG(std::to_string(moveSpeed_));
        DEBUG_LOG(std::to_string(direction_));

        if (not backwardAnimationName_.empty())
        {
            animator_.ChangeAnimation(backwardAnimationName_, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, std::nullopt);
        }
        else
        {
            if (not forwardAnimationName_.empty())
            {
                animator_.ChangeAnimation(forwardAnimationName_, Animator::AnimationChangeType::smooth,
                                          PlayDirection::backward, std::nullopt);
            }
        }
    }

    void update(float deltaTime)
    {
        auto targetVelocity = rigidbody_.GetRotation() * direction_;
        targetVelocity      = glm::normalize(targetVelocity);
        targetVelocity *= moveSpeed_;

        auto velocity       = rigidbody_.GetVelocity();
        auto velocityChange = (targetVelocity - velocity);
        velocityChange.x    = glm::clamp(velocityChange.x, -moveSpeed_, moveSpeed_);
        velocityChange.z    = glm::clamp(velocityChange.z, -moveSpeed_, moveSpeed_);
        velocityChange.y    = 0;

        auto newVelocity = velocity + velocityChange;
        rigidbody_.SetVelocity(newVelocity);
    }

    Rigidbody& rigidbody_;
    Animator& animator_;
    const std::string& forwardAnimationName_;
    const std::string& backwardAnimationName_;

    vec3 direction_;
    float moveSpeed_;
};
}  // namespace Components
}  // namespace GameEngine
