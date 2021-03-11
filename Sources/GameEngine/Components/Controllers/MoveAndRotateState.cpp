#include "MoveAndRotateState.h"

#include "CommonActions.h"
#include "MoveState.h"
#include "RotateState.h"

namespace GameEngine
{
namespace Components
{
MoveAndRotateState::MoveAndRotateState(FsmContext &context)
    : context_{context}
{
}

void MoveAndRotateState::onEnter(const RotateLeftEvent &event)
{
    DEBUG_LOG("onEnter(const RotateLeftEvent& )");
    context_.rotationSpeed = fabsf(event.speed);
}

void MoveAndRotateState::onEnter(const RotateRightEvent &event)
{
    DEBUG_LOG("onEnter(const RotateRightEvent& )");
    context_.rotationSpeed = -fabsf(event.speed);
}

void MoveAndRotateState::onEnter(const RotateTargetEvent &)
{
    DEBUG_LOG("onEnter(const RotateTargetEvent& )");
}

void MoveAndRotateState::onEnter(const MoveForwardEvent &event)
{
    DEBUG_LOG("onEnter(const MoveForwardEvent&)");
    context_.moveDirection = vec3(0.f, 0.f, 1.f);
    context_.moveSpeed     = fabsf(event.moveSpeed);

    if (not context_.forwardAnimationName.empty())
    {
        context_.animator.ChangeAnimation(context_.forwardAnimationName, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
}

void MoveAndRotateState::onEnter(const MoveBackwardEvent &event)
{
    DEBUG_LOG("onEnter(const MoveBackwardEvent&)");

    context_.moveDirection = vec3(0.f, 0.f, -1.f);
    context_.moveSpeed     = fabsf(event.moveSpeed);

    if (not context_.backwardAnimationName.empty())
    {
        context_.animator.ChangeAnimation(context_.backwardAnimationName, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, std::nullopt);
    }
    else
    {
        if (not context_.forwardAnimationName.empty())
        {
            context_.animator.ChangeAnimation(context_.forwardAnimationName, Animator::AnimationChangeType::smooth,
                                              PlayDirection::backward, std::nullopt);
        }
    }
}

void MoveAndRotateState::update(float deltaTime)
{
    moveRigidbody(context_);

    auto rotation = context_.rigidbody.GetRotation() *
                    glm::angleAxis(glm::radians(context_.rotationSpeed * deltaTime), glm::vec3(0.f, 1.f, 0.f));
    context_.rigidbody.SetRotation(rotation);
}

}  // namespace Components
}  // namespace GameEngine
