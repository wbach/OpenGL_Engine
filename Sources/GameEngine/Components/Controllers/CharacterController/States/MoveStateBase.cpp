#include "MoveStateBase.h"

#include <Types.h>
#include <Utils/Fsm/Actions.h>

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
                             const MoveSpeed &moveSpeed, const std::string &forwardAnimName,
                             const std::string &backwardAnimName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , animationClips_{forwardAnimName, backwardAnimName}
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

void MoveStateBase::onEnter()
{
    isAnimationReady = false;
}

void MoveStateBase::onEnter(const EquipEndStateEvent &)
{
    updateMoveState();
}

void MoveStateBase::onEnter(const DisarmEndStateEvent &)
{
    updateMoveState();
}

void MoveStateBase::onEnter(const SprintStartEvent &)
{
    DEBUG_LOG("onEnter(const SprintStartEvent &)");
    moveForward();
}

void MoveStateBase::onEnter(const SprintStateChangeEvent &)
{
    // Sprint is only for forward move
    moveForward();
}

void MoveStateBase::onEnter(const WalkForwardEvent &)
{
    moveForward();
}

void MoveStateBase::onEnter(const WalkBackwardEvent &)
{
    DEBUG_LOG("onEnter(const WalkBackwardEvent &)");
    moveBackward();
}
void MoveStateBase::onEnter(const RunForwardEvent &)
{
    moveForward();
}
void MoveStateBase::onEnter(const RunBackwardEvent &)
{
    moveBackward();
}
void MoveStateBase::onEnter(const EndJumpEvent &)
{
    setCurrentAnim();
}

void MoveStateBase::onEnter(const WalkChangeStateEvent &)
{
    updateMoveState();
}

void MoveStateBase::onEnter(const RunLeftEvent &)
{
    moveLeft();
}

void MoveStateBase::onEnter(const RunRightEvent &)
{
    moveRight();
}

void MoveStateBase::onEnter(const WalkLeftEvent &)
{
    moveLeft();
}

void MoveStateBase::onEnter(const WalkRightEvent &)
{
    moveRight();
}

bool MoveStateBase::transitionCondition(const EndForwardMoveEvent &)
{
    context_.moveStateData_.currentEvents_.erase(FsmContext::MoveStateData::DirectionEvents::Forward);

    if (context_.moveStateData_.isEventActive(FsmContext::MoveStateData::DirectionEvents::Backward))
    {
        if (context_.moveStateData_.isMovingInDirection(FsmContext::MoveStateData::DirectionEvents::Forward))
        {
            moveBackward();
        }
    }
    else
    {
        context_.moveStateData_.moveDirection.z = 0.f;
    }

    return context_.moveStateData_.currentEvents_.empty();
}
bool MoveStateBase::transitionCondition(const EndBackwardMoveEvent &)
{
    context_.moveStateData_.currentEvents_.erase(FsmContext::MoveStateData::DirectionEvents::Backward);

    if (context_.moveStateData_.isEventActive(FsmContext::MoveStateData::DirectionEvents::Forward))
    {
        if (context_.moveStateData_.isMovingInDirection(FsmContext::MoveStateData::DirectionEvents::Backward))
        {
            moveForward();
        }
    }
    else
    {
        context_.moveStateData_.moveDirection.z = 0.f;
    }

    return context_.moveStateData_.currentEvents_.empty();
}

bool MoveStateBase::transitionCondition(const EndMoveLeftEvent &)
{
    context_.moveStateData_.currentEvents_.erase(FsmContext::MoveStateData::DirectionEvents::Left);

    if (context_.moveStateData_.isEventActive(FsmContext::MoveStateData::DirectionEvents::Right))
    {
        if (context_.moveStateData_.isMovingInDirection(FsmContext::MoveStateData::DirectionEvents::Left))
        {
            moveRight();
        }
    }
    else
    {
        context_.moveStateData_.moveDirection.x = 0.f;
    }
    return context_.moveStateData_.currentEvents_.empty();
}

bool MoveStateBase::transitionCondition(const EndMoveRightEvent &)
{
    context_.moveStateData_.currentEvents_.erase(FsmContext::MoveStateData::DirectionEvents::Right);

    if (context_.moveStateData_.isEventActive(FsmContext::MoveStateData::DirectionEvents::Left))
    {
        if (context_.moveStateData_.isMovingInDirection(FsmContext::MoveStateData::DirectionEvents::Right))
        {
            moveLeft();
        }
    }
    else
    {
        context_.moveStateData_.moveDirection.x = 0.f;
    }
    return context_.moveStateData_.currentEvents_.empty();
}

bool MoveStateBase::transitionCondition(const SprintStartEvent &)
{
    return context_.moveStateData_.moveDirection.z > -0.001f;
}

bool MoveStateBase::transitionCondition(const SprintStateChangeEvent &)
{
    return context_.moveStateData_.moveDirection.z > 0.01f;
}

void MoveStateBase::onLeave()
{
    //    context_.moveStateData_.isForwardEvent_  = false;
    //    context_.moveStateData_.isBackwardEvent_ = false;
}

void MoveStateBase::changeAnimationClips(const MovmentClipNames &clips)
{
    if (clips != animationClips_)
    {
        animationClips_ = clips;
        setCurrentAnim();
    }
}

void MoveStateBase::moveForward()
{
    setMoveForwardData();
    setForwardAnim();
}

void MoveStateBase::moveBackward()
{
    setMoveBackwardData();
    setBackwardAnim();
}

void MoveStateBase::moveLeft()
{
    setMoveLeftData();
    setLeftAnim();
}

void MoveStateBase::moveRight()
{
    setMoveRightData();
    setRightAnim();
}

void MoveStateBase::setMoveForwardData()
{
    context_.moveStateData_.currentEvents_.insert(FsmContext::MoveStateData::DirectionEvents::Forward);
    context_.moveStateData_.moveDirection.z   = 1.f;
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);
}

void MoveStateBase::setMoveBackwardData()
{
    context_.moveStateData_.currentEvents_.insert(FsmContext::MoveStateData::DirectionEvents::Backward);
    context_.moveStateData_.moveDirection.z   = -1.f;
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.backward);
}

void MoveStateBase::setMoveLeftData()
{
    context_.moveStateData_.currentEvents_.insert(FsmContext::MoveStateData::DirectionEvents::Left);
    context_.moveStateData_.moveDirection.x   = 1.f;
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.leftRight);
}

void MoveStateBase::setMoveRightData()
{
    context_.moveStateData_.currentEvents_.insert(FsmContext::MoveStateData::DirectionEvents::Right);
    context_.moveStateData_.moveDirection.x   = -1.f;
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.leftRight);
}

void MoveStateBase::updateMoveState()
{
    setCurrentAnim();
    setCurrentMoveSpeed();
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
    moveRigidbody(context_);
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

void MoveStateBase::setForwardAnim()
{
    setAnim(animationClips_.forward);
}

void MoveStateBase::setBackwardAnim()
{
    if (not animationClips_.backward.empty() and not context_.animator.isAnimationPlaying(animationClips_.backward))
    {
        context_.animator.ChangeAnimation(animationClips_.backward, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_);
    }
    else if (not animationClips_.forward.empty())
    {
        context_.animator.ChangeAnimation(animationClips_.forward, Animator::AnimationChangeType::smooth,
                                          PlayDirection::backward, jointGroupName_);
    }
}

void MoveStateBase::setLeftAnim()
{
    setAnim(animationClips_.moveleft);
}

void MoveStateBase::setRightAnim()
{
    setAnim(animationClips_.moveRight);
}

void MoveStateBase::setCurrentAnim()
{
    if (context_.moveStateData_.moveDirection.z > 0.01f)
    {
        setForwardAnim();
    }
    else if (context_.moveStateData_.moveDirection.z < -0.01f)
    {
        setBackwardAnim();
    }
}
void MoveStateBase::setCurrentMoveSpeed()
{
    if (context_.moveStateData_.moveDirection.z > 0.01f)
    {
        context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);
    }
    else if (context_.moveStateData_.moveDirection.z < -0.01f)
    {
        context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.backward);
    }
}

void MoveStateBase::setAnim(const std::string &clipName)
{
    if (not clipName.empty() and not context_.animator.isAnimationPlaying(clipName))
    {
        context_.animator.ChangeAnimation(clipName, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                          jointGroupName_);
    }
}

void MoveStateBase::moveRigidbody(FsmContext &context)
{
    if (not isAnimationReady)
    {
        isAnimationReady = context_.animator.isAnimationPlaying(animationClips_.forward) or
                           context_.animator.isAnimationPlaying(animationClips_.backward) or
                           context_.animator.isAnimationPlaying(animationClips_.moveleft) or
                           context_.animator.isAnimationPlaying(animationClips_.moveRight);
    }
    if (isAnimationReady)
    {
        const auto &moveDirection = context.moveStateData_.moveDirection;
        const auto &moveSpeed     = context.moveStateData_.currentMoveSpeed_;
        auto &rigidbody           = context.rigidbody;

        DEBUG_LOG("Dir : " + std::to_string(moveDirection));
        auto targetVelocity = glm::normalize(rigidbody.GetRotation() * moveDirection) * moveSpeed;

        auto velocity       = rigidbody.GetVelocity();
        auto velocityChange = (targetVelocity - rigidbody.GetVelocity());

        velocityChange.x = glm::clamp(velocityChange.x, -moveSpeed, moveSpeed);
        velocityChange.z = glm::clamp(velocityChange.z, -moveSpeed, moveSpeed);
        velocityChange.y = 0;

        auto newVelocity = velocity + velocityChange;
        rigidbody.SetVelocity(newVelocity);
    }
}

}  // namespace Components
}  // namespace GameEngine
