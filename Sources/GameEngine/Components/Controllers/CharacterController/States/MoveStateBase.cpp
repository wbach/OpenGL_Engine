#include "MoveStateBase.h"

#include <GLM/GLMUtils.h>
#include <Types.h>

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
MoveStateBase::MoveStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName, const MoveSpeed &moveSpeed,
                             const MovmentClipNames &clips)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , animationClips_{clips}
    , moveSpeed_{moveSpeed}
{
}

MoveStateBase::MoveStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName, float forwardMoveSpeed,
                             const std::string &forwardAnimName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , animationClips_{forwardAnimName}
    , moveSpeed_{forwardMoveSpeed, 0.0f, 0.0f}
{
}

void MoveStateBase::onEnter(const CrouchChangeStateEvent &)
{
    setCurrentAnimIfNeeded();
}

void MoveStateBase::onEnter(const SprintStateChangeEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("SprintStateChangeEvent");
}

void MoveStateBase::onEnter(const MoveEvent &)
{
    setCurrentAnimIfNeeded();
}

void MoveStateBase::onEnter(const MoveForwardEvent &)
{
    context_.moveController.moveForwad();
}
void MoveStateBase::onEnter(const MoveBackwardEvent &)
{
    context_.moveController.moveBackward();
}
void MoveStateBase::onEnter(const MoveLeftEvent &)
{
    context_.moveController.moveLeft();
}

void MoveStateBase::onEnter(const MoveRightEvent &)
{
    context_.moveController.moveRight();
}

void MoveStateBase::onEnter(const GroundDetectionEvent &)
{
}

void MoveStateBase::onEnter(const WalkChangeStateEvent &)
{
}

void MoveStateBase::postEnter()
{
    // /*DISABLED*/ DEBUG_LOG("postEnter");
    setCurrentAnimIfNeeded();
}

bool MoveStateBase::transitionCondition(const EndForwardMoveEvent &)
{
    context_.moveController.endMoveForwad();
    return shouldLeaveAndSetCurrAnimIfNot();
}
bool MoveStateBase::transitionCondition(const EndBackwardMoveEvent &)
{
    context_.moveController.endMoveBackward();
    return shouldLeaveAndSetCurrAnimIfNot();
}

bool MoveStateBase::transitionCondition(const EndMoveLeftEvent &)
{
    context_.moveController.endMoveLeft();
    return shouldLeaveAndSetCurrAnimIfNot();
}

bool MoveStateBase::transitionCondition(const EndMoveRightEvent &)
{
    context_.moveController.endMoveRight();
    return shouldLeaveAndSetCurrAnimIfNot();
}

void MoveStateBase::onLeave()
{
}

void MoveStateBase::changeAnimationClips(const MovmentClipNames &clips)
{
    if (clips != animationClips_)
    {
        animationClips_ = clips;
    }
}

void MoveStateBase::onMoveInactivity()
{
}

bool MoveStateBase::shouldLeaveAndSetCurrAnimIfNot()
{
    if (context_.moveController.isMoveActive())
    {
        // /*DISABLED*/ DEBUG_LOG("shouldLeaveAndSetCurrAnimIfNot");
        setCurrentAnimIfNeeded();
        return false;
    }
    return true;
}

void MoveStateBase::update(const MoveForwardEvent &event)
{
    onEnter(event);
}
void MoveStateBase::update(const MoveBackwardEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const MoveLeftEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const MoveRightEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const SprintStateChangeEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(float dt)
{
    // /*DISABLED*/ DEBUG_LOG("Update");
    if (not context_.animator.isAnimationPlaying(currentAnimName_))
    {
        // // /*DISABLED*/ DEBUG_LOG("Forward not ready");
        return;
    }

    moveRigidbody();
}

void MoveStateBase::postUpdate()
{
    // /*DISABLED*/ DEBUG_LOG("postUpdate");
    setCurrentAnimIfNeeded();
}

void MoveStateBase::setCurrentAnimIfNeeded()
{
    if (context_.moveController.isMoving(MoveController::Direction::Forward))
    {
        setAnim(animationClips_.forward);
    }
    else if (context_.moveController.isMoving(MoveController::Direction::Backward))
    {
        setAnim(animationClips_.backward);
    }
    else if (context_.moveController.isMoving(MoveController::Direction::Left))
    {
        setAnim(animationClips_.left);
    }
    else if (context_.moveController.isMoving(MoveController::Direction::Right))
    {
        setAnim(animationClips_.right);
    }
    else
    {
        onMoveInactivity();
    }
}

void MoveStateBase::setAnim(const std::string &clipName)
{
    if (clipName.empty())
    {
        context_.animator.StopAnimation(jointGroupName_);
        return;
    }
    currentAnimName_ = clipName;

    auto currentAnimations = context_.animator.getCurrentAnimationName();
    auto iter              = std::find(currentAnimations.begin(), currentAnimations.end(), clipName);
    if (iter != currentAnimations.end())
    {
        DEBUG_LOG("current is playing");
        return;
    }

    // // /*DISABLED*/ DEBUG_LOG("SetAnim : " + clipName + " jointGroup=" + std::to_string(jointGroupName_));
    context_.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward, jointGroupName_);
}

void MoveStateBase::moveRigidbody()
{
    if (not context_.moveController.isMoving())
    {
        // // /*DISABLED*/ DEBUG_LOG("Not moving, return");
        return;
    }

    auto moveDirection = glm::normalize(context_.moveController.getCurrentDir());
    auto moveSpeed     = glm::length(
            vec3(moveSpeed_.leftRight, 0, moveDirection.z > 0.5f ? moveSpeed_.forward : moveSpeed_.backward) * moveDirection);

    if (moveSpeed < 0.00001f)
    {
        return;
    }

    auto &rigidbody     = context_.rigidbody;
    auto targetVelocity = rigidbody.GetRotation() * moveDirection * moveSpeed;
    targetVelocity.y    = rigidbody.GetVelocity().y;
    // /*DISABLED*/ DEBUG_LOG("moveDirection : " + std::to_string(moveDirection));
    // /*DISABLED*/ DEBUG_LOG("moveSpeed : " + std::to_string(moveSpeed));
    // /*DISABLED*/ DEBUG_LOG("targetVelocity : " + std::to_string(targetVelocity));
    rigidbody.SetVelocity(targetVelocity);
}

}  // namespace Components
}  // namespace GameEngine
