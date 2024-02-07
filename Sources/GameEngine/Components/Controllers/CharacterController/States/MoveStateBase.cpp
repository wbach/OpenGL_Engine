#include "MoveStateBase.h"

#include <Types.h>
#include <Utils/Fsm/Actions.h>

namespace GameEngine
{
namespace Components
{
MoveStateBase::MoveStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName,
                             const MoveSpeed &moveSpeed, const std::string &forwardAnimName,
                             const std::string &backwardAnimName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , forwardAnimName_{forwardAnimName}
    , backwardAnimName_{backwardAnimName}
    , moveSpeed_{moveSpeed}
{
}

MoveStateBase::MoveStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName,
                             float forwardMoveSpeed, const std::string &forwardAnimName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , forwardAnimName_{forwardAnimName}
    , moveSpeed_{forwardMoveSpeed, 0.0f, 0.0f}
{
}

void MoveStateBase::onEnter(const EquipEndStateEvent &)
{
    updateMoveState();
}

void MoveStateBase::onEnter(const DisarmEndStateEvent&)
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
        moveBackward();
    }

    return not context_.moveStateData_.isBackwardEvent_;
}
bool MoveStateBase::transitionCondition(const EndBackwardMoveEvent &)
{
    context_.moveStateData_.isBackwardEvent_ = false;

    if (context_.moveStateData_.isForwardEvent_ and context_.moveDirection.z < -0.5f)
    {
        moveForward();
    }
    return not context_.moveStateData_.isForwardEvent_;
}

bool MoveStateBase::transitionCondition(const SprintStartEvent &)
{
    return context_.moveDirection.z > -0.001f;
}

bool MoveStateBase::transitionCondition(const SprintStateChangeEvent &)
{
    return context_.moveDirection.z > 0.01f;
}
void MoveStateBase::onLeave()
{
    //    context_.moveStateData_.isForwardEvent_  = false;
    //    context_.moveStateData_.isBackwardEvent_ = false;
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

void MoveStateBase::setMoveForwardData()
{
    context_.moveStateData_.isForwardEvent_ = true;

    context_.moveDirection                    = vec3(0.f, 0.f, 1.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);
}

void MoveStateBase::setMoveBackwardData()
{
    context_.moveStateData_.isBackwardEvent_  = true;
    context_.moveDirection                    = vec3(0.f, 0.f, -1.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.backward);
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

void MoveStateBase::setForwardAnim()
{
    if (not forwardAnimName_.empty())
    {
        context_.moveStateData_.animationIsReady_ = false;
        context_.animator.ChangeAnimation(forwardAnimName_, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_, [this]() {
                                              DEBUG_LOG("animationIsReady_ = true");
                                              context_.moveStateData_.animationIsReady_ = true;
                                          });
    }
}

void MoveStateBase::setBackwardAnim()
{
    if (not backwardAnimName_.empty())
    {
        context_.moveStateData_.animationIsReady_ = false;
        context_.animator.ChangeAnimation(backwardAnimName_, Animator::AnimationChangeType::smooth,
                                          PlayDirection::forward, jointGroupName_,
                                          [this]() { context_.moveStateData_.animationIsReady_ = true; });
    }
    else if (not forwardAnimName_.empty())
    {
        context_.moveStateData_.animationIsReady_ = false;
        context_.animator.ChangeAnimation(forwardAnimName_, Animator::AnimationChangeType::smooth,
                                          PlayDirection::backward, jointGroupName_,
                                          [this]() { context_.moveStateData_.animationIsReady_ = true; });
    }
}

void MoveStateBase::setCurrentAnim()
{
    if (context_.moveDirection.z > 0.01f)
    {
        setForwardAnim();
    }
    else if (context_.moveDirection.z < -0.01f)
    {
        setBackwardAnim();
    }
}
void MoveStateBase::setCurrentMoveSpeed()
{
    if (context_.moveDirection.z > 0.01f)
    {
        context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);
    }
    else if (context_.moveDirection.z < -0.01f)
    {
        context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.backward);
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
        velocityChange.x    = glm::clamp(velocityChange.x, -context_.moveStateData_.currentMoveSpeed_,
                                      context_.moveStateData_.currentMoveSpeed_);
        velocityChange.z    = glm::clamp(velocityChange.z, -context_.moveStateData_.currentMoveSpeed_,
                                      context_.moveStateData_.currentMoveSpeed_);
        velocityChange.y    = 0;

        auto newVelocity = velocity + velocityChange;
        DEBUG_LOG("Set speed: " + std::to_string(context_.moveStateData_.currentMoveSpeed_));
        DEBUG_LOG("SetVelocity: " + std::to_string(newVelocity));
        rigidbody.SetVelocity(newVelocity);
    }
    else
    {
        DEBUG_LOG("not context_.moveStateData_.animationIsReady_!");
    }
}

}  // namespace Components
}  // namespace GameEngine
