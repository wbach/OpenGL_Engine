#include "MoveStateBase.h"

#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveStateBase::MoveStateBase(FsmContext &context, const std::string &forwardAnimName,
                             const std::string &backwardAnimName)
    : context_{context}
    , forwardAnimName_{forwardAnimName}
    , backwardAnimName_{backwardAnimName}
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
    context_.moveDirection = vec3(0.f, 0.f, 1.f);
    context_.moveSpeed     = fabsf(event.moveSpeed);

    setForwardAnim();
}
void MoveStateBase::onEnter(const MoveBackwardEvent &event)
{
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
    if (not forwardAnimName_.empty())
    {
        animationIsReady_ = false;
        context_.animator.ChangeAnimation(
            forwardAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt,
            [this]() { animationIsReady_ = true; });
    }
}

void MoveStateBase::setBackwardAnim()
{
    if (not backwardAnimName_.empty())
    {
        animationIsReady_ = false;
        context_.animator.ChangeAnimation(
            backwardAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt,
            [this]() { animationIsReady_ = true; });
    }
    else if (not forwardAnimName_.empty())
    {
        animationIsReady_ = false;
        context_.animator.ChangeAnimation(
            forwardAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::backward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt,
            [this]() { animationIsReady_ = true; });
    }
}
void MoveStateBase::moveRigidbody(FsmContext &context)
{
    if (animationIsReady_)
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
}

}  // namespace Components
}  // namespace GameEngine
