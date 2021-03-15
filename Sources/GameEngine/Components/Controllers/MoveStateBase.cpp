#include "MoveStateBase.h"

#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveStateBase::MoveStateBase(FsmContext &context)
    : context_{context}
{
}
void MoveStateBase::onEnter(const EndJumpEvent &)
{
    if (context_.moveDirection.z > 0)
    {
        setForwardAnim();
    }
    else
    {
        setBackwardAnim();
    }
}
void MoveStateBase::onEnter(const MoveForwardEvent &event)
{
    DEBUG_LOG("onEnter(const MoveForwardEvent&)");

    context_.moveDirection = vec3(0.f, 0.f, 1.f);
    context_.moveSpeed     = fabsf(event.moveSpeed);

    setForwardAnim();
}
void MoveStateBase::onEnter(const MoveBackwardEvent &event)
{
    DEBUG_LOG("onEnter(const MoveBackwardEvent&)");

    context_.moveDirection = vec3(0.f, 0.f, -1.f);
    context_.moveSpeed     = fabsf(event.moveSpeed);

    setBackwardAnim();
}
void MoveStateBase::update(const AttackEvent &)
{
    context_.multiAnimations = true;
    context_.attackFsm.handle(AttackFsmEvents::AttackGrouped{context_.upperBodyGroupName});
}
void MoveStateBase::update(const EndAttackEvent &)
{
    context_.multiAnimations = false;
    context_.attackFsm.handle(AttackFsmEvents::End{});
}
void MoveStateBase::update(float)
{
    moveRigidbody(context_);
}

void MoveStateBase::setForwardAnim()
{
    if (not context_.forwardAnimationName.empty())
    {
        context_.animator.ChangeAnimation(
            context_.forwardAnimationName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
}

void MoveStateBase::setBackwardAnim()
{
    if (not context_.backwardAnimationName.empty())
    {
        context_.animator.ChangeAnimation(
            context_.backwardAnimationName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
    }
    else
    {
        if (not context_.forwardAnimationName.empty())
        {
            context_.animator.ChangeAnimation(
                context_.forwardAnimationName, Animator::AnimationChangeType::smooth, PlayDirection::backward,
                context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt);
        }
    }
}
void MoveStateBase::moveRigidbody(FsmContext &context)
{
    const auto &moveDirection = context.moveDirection;
    const auto &moveSpeed     = context.moveSpeed;
    auto &rigidbody           = context.rigidbody;

    auto targetVelocity = rigidbody.GetRotation() * moveDirection;
    targetVelocity      = glm::normalize(targetVelocity);
    targetVelocity *= moveSpeed;

    auto velocity       = rigidbody.GetVelocity();
    auto velocityChange = (targetVelocity - velocity);
    velocityChange.x    = glm::clamp(velocityChange.x, -moveSpeed, moveSpeed);
    velocityChange.z    = glm::clamp(velocityChange.z, -moveSpeed, moveSpeed);
    velocityChange.y    = 0;

    auto newVelocity = velocity + velocityChange;
    rigidbody.SetVelocity(newVelocity);
}

}  // namespace Components
}  // namespace GameEngine
