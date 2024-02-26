#include "MoveStateBase.h"

#include <GLM/GLMUtils.h>
#include <Types.h>
#include <Utils/Fsm/Actions.h>

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
MoveStateBase::MoveStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName,
                             const MoveSpeed &moveSpeed, const MovmentClipNames &clips)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , animationClips_{clips}
    , moveSpeed_{moveSpeed}
{
}

MoveStateBase::MoveStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName,
                             float forwardMoveSpeed, const std::string &forwardAnimName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , animationClips_{forwardAnimName}
    , moveSpeed_{forwardMoveSpeed, 0.0f, 0.0f}
{
}

void MoveStateBase::onEnter(const SprintStartEvent &)
{
    DEBUG_LOG("onEnter(const SprintStartEvent &)");
    context_.moveController.moveForwad();
}

void MoveStateBase::onEnter(const SprintStateChangeEvent &)
{
    // Sprint is only for forward move
    context_.moveController.moveForwad();
}

void MoveStateBase::onEnter(const WalkForwardEvent &)
{
    context_.moveController.moveForwad();
}

void MoveStateBase::onEnter(const WalkBackwardEvent &)
{
    context_.moveController.moveBackward();
}
void MoveStateBase::onEnter(const RunForwardEvent &)
{
    context_.moveController.moveForwad();
}
void MoveStateBase::onEnter(const RunBackwardEvent &)
{
    context_.moveController.moveBackward();
}
void MoveStateBase::onEnter(const EndJumpEvent &)
{
}

void MoveStateBase::onEnter(const WalkChangeStateEvent &)
{
}

void MoveStateBase::postEnter()
{
    DEBUG_LOG("postEnter");
    setCurrentAnimIfNeeded();
}

void MoveStateBase::onEnter(const RunLeftEvent &)
{
    context_.moveController.moveLeft();
}

void MoveStateBase::onEnter(const RunRightEvent &)
{
    context_.moveController.moveRight();
}

void MoveStateBase::onEnter(const WalkLeftEvent &)
{
    context_.moveController.moveLeft();
}

void MoveStateBase::onEnter(const WalkRightEvent &)
{
    context_.moveController.moveRight();
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

bool MoveStateBase::transitionCondition(const SprintStartEvent &)
{
    return context_.moveController.isMoving(MoveController::Direction::Forward);
}

bool MoveStateBase::transitionCondition(const SprintStateChangeEvent &)
{
    return context_.moveController.isMoving(MoveController::Direction::Forward);
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
        setCurrentAnimIfNeeded();
        return false;
    }
    return true;
}

void MoveStateBase::update(const RunForwardEvent &event)
{
    onEnter(event);
}
void MoveStateBase::update(const RunBackwardEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const WalkForwardEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const WalkBackwardEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const SprintStateChangeEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const SprintStartEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(float)
{
    moveRigidbody();
}

void MoveStateBase::postUpdate()
{
    setCurrentAnimIfNeeded();
}

void MoveStateBase::update(const RunLeftEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const RunRightEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const WalkRightEvent &event)
{
    onEnter(event);
}

void MoveStateBase::update(const WalkLeftEvent &event)
{
    onEnter(event);
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
        setAnim(animationClips_.moveleft);
    }
    else if (context_.moveController.isMoving(MoveController::Direction::Right))
    {
        setAnim(animationClips_.moveRight);
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
        return;

    //DEBUG_LOG("SetAnim : " + clipName + " jointGroup=" + std::to_string(jointGroupName_));
    context_.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                      jointGroupName_);
}

void MoveStateBase::moveRigidbody()
{
    if (not context_.moveController.isMoving())
    {
        //DEBUG_LOG("Not moving, return");
        return;
    }

    if (not context_.animator.isAnimationPlaying(currentAnimName_))
    {
        //DEBUG_LOG("Forward not ready");
        return;
    }

    const auto &moveDirection = glm::normalize(context_.moveController.getCurrentDir());
    auto moveSpeed = glm::length(vec3(moveSpeed_.leftRight, 0, moveDirection.z > 0.5f ? moveSpeed_.forward : moveSpeed_.backward) * moveDirection);

    auto &rigidbody     = context_.rigidbody;
    auto targetVelocity = rigidbody.GetRotation() * moveDirection * moveSpeed;
    targetVelocity.y = rigidbody.GetVelocity().y;
//    DEBUG_LOG("moveDirection : " + std::to_string(moveDirection));
//    DEBUG_LOG("moveSpeed : " + std::to_string(moveSpeed));
//    DEBUG_LOG("targetVelocity : " + std::to_string(targetVelocity));
    rigidbody.SetVelocity(targetVelocity);
}

}  // namespace Components
}  // namespace GameEngine
