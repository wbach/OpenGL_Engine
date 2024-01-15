#include "MoveStateBase.h"

#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include "CharacterControllerEvents.h"
#include "FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveStateBase::MoveStateBase(FsmContext &context, const MoveSpeed &moveSpeed, const std::string &forwardAnimName,
                             const std::string &backwardAnimName)
    : context_{context}
    , forwardAnimName_{forwardAnimName}
    , backwardAnimName_{backwardAnimName}
    , moveSpeed_{moveSpeed}
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
void MoveStateBase::onEnter(const MoveForwardEvent &)
{
    setMoveForward();
    setForwardAnim();
}
void MoveStateBase::onEnter(const MoveBackwardEvent &)
{
    setMoveBackward();
    setBackwardAnim();
}

void MoveStateBase::onEnter(const MoveLeftEvent &)
{
    context_.moveDirection                    = vec3(1.f, 0.f, 0.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.leftRight);

    // setLeftAnim();
}

void MoveStateBase::onEnter(const MoveRightEvent &)
{
    context_.moveDirection                    = vec3(-1.f, 0.f, 0.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.leftRight);

    // setRightAnim();
}
bool MoveStateBase::transitionCondition(const EndForwardMoveEvent &)
{
    context_.moveStateData_.isForwardEvent_ = false;

    if (context_.moveStateData_.isBackwardEvent_ and context_.moveDirection.z > 0.5f)
    {
        onEnter(MoveBackwardEvent());
    }

    return not context_.moveStateData_.isBackwardEvent_;
}
bool MoveStateBase::transitionCondition(const EndBackwardMoveEvent &)
{
    context_.moveStateData_.isBackwardEvent_ = false;

    if (context_.moveStateData_.isForwardEvent_ and context_.moveDirection.z < -0.5f)
    {
        onEnter(MoveForwardEvent());
    }
    return not context_.moveStateData_.isForwardEvent_;
}
void MoveStateBase::onLeave()
{
//    context_.moveStateData_.isForwardEvent_  = false;
//    context_.moveStateData_.isBackwardEvent_ = false;
}

void MoveStateBase::setMoveForward()
{
    context_.moveStateData_.isForwardEvent_ = true;

    context_.moveDirection  = vec3(0.f, 0.f, 1.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);
}

void MoveStateBase::setMoveBackward()
{
    context_.moveStateData_.isBackwardEvent_  = true;
    context_.moveDirection  = vec3(0.f, 0.f, -1.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.backward);
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
void MoveStateBase::update(const MoveForwardEvent &event)
{
    onEnter(event);
}
void MoveStateBase::update(const MoveBackwardEvent &event)
{
    onEnter(event);
}
void MoveStateBase::update(float)
{
    moveRigidbody(context_);
}

void MoveStateBase::setForwardAnim()
{
    if (not forwardAnimName_.empty())
    {
        context_.moveStateData_.animationIsReady_ = false;
        context_.animator.ChangeAnimation(
            forwardAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt,
            [this]() { context_.moveStateData_.animationIsReady_ = true; });
    }
}

void MoveStateBase::setBackwardAnim()
{
    if (not backwardAnimName_.empty())
    {
        context_.moveStateData_.animationIsReady_ = false;
        context_.animator.ChangeAnimation(
            backwardAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt,
            [this]() { context_.moveStateData_.animationIsReady_ = true; });
    }
    else if (not forwardAnimName_.empty())
    {
        context_.moveStateData_.animationIsReady_ = false;
        context_.animator.ChangeAnimation(
            forwardAnimName_, Animator::AnimationChangeType::smooth, PlayDirection::backward,
            context_.multiAnimations ? std::make_optional(context_.lowerBodyGroupName) : std::nullopt,
            [this]() { context_.moveStateData_.animationIsReady_ = true; });
    }
}
void MoveStateBase::moveRigidbody(FsmContext &context)
{
    if (context_.moveStateData_.animationIsReady_)
    {
        const auto &moveDirection = context.moveDirection;
        auto &rigidbody           = context.rigidbody;

        auto targetVelocity = rigidbody.GetRotation() * moveDirection;
        targetVelocity      = glm::normalize(targetVelocity);
        targetVelocity *= context_.moveStateData_.currentMoveSpeed_;

        auto velocity       = rigidbody.GetVelocity();
        auto velocityChange = (targetVelocity - velocity);
        velocityChange.x    = glm::clamp(velocityChange.x, -context_.moveStateData_.currentMoveSpeed_, context_.moveStateData_.currentMoveSpeed_);
        velocityChange.z    = glm::clamp(velocityChange.z, -context_.moveStateData_.currentMoveSpeed_, context_.moveStateData_.currentMoveSpeed_);
        velocityChange.y    = 0;

        auto newVelocity = velocity + velocityChange;
        rigidbody.SetVelocity(newVelocity);
    }
}

}  // namespace Components
}  // namespace GameEngine
