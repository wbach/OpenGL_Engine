#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
RotatingMoveState::RotatingMoveState(FsmContext &context, const std::optional<std::string> &jointGroup, float moveSpeed,
                                     const std::string &forwardAnimName)
    : MoveStateBase(context, jointGroup, moveSpeed, forwardAnimName)
{
    moveSpeed_.leftRight = moveSpeed;
    moveSpeed_.backward  = moveSpeed;
}

void RotatingMoveState::onEnter(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::onEnter(const RunBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::onEnter(const RunLeftEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::onEnter(const RunRightEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::update(float dt)
{
    moveRigidbody();

    if (context_.progress < 1.f)
    {
        const float rotateTime{0.5f};
        context_.progress += dt / rotateTime;
        applyCurrentRotation();
    }
    else
    {
        context_.progress = 1.f;
    }
}

void RotatingMoveState::update(const RunForwardEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::update(const RunBackwardEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::update(const RunLeftEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::update(const RunRightEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::postEnter()
{
    // prevent to call postEnter from MoveStateBase
    setTargetAngle();
    setAnim(animationClips_.forward);
}

void RotatingMoveState::postUpdate()
{
    setTargetAngle();
    if (not context_.moveController.isMoving())
    {
         onMoveInactivity();
    }
}

void RotatingMoveState::onLeave()
{
}

bool RotatingMoveState::shouldLeaveAndSetCurrAnimIfNot()
{
    if (context_.moveController.isMoveActive())
    {
        if (context_.moveController.isMoving())
        {
            setTargetAngle();
            setAnim(animationClips_.forward);
        }
        else
        {
            onMoveInactivity();
        }
        return false;
    }
    return true;
}

void RotatingMoveState::setCharacterRotation(const mat4 &matrixRotation)
{
    auto rj = context_.animator.GetRootJoint();
    if (rj)
    {
        rj->additionalUserMofiyTransform.set(matrixRotation);
    }
    else
    {
        DEBUG_LOG("not setCharacterRotation");
    }
}

void RotatingMoveState::setTargetAngle()
{
    DEBUG_LOG("Current dir : " + std::to_string(context_.moveController.getCurrentDir()));
    context_.progress = 0.f;
    context_.sourceAngle = context_.currentAngle;
    context_.targetAngle = glm::angleAxis(
        glm::orientedAngle(VECTOR_FORWARD, glm::normalize(context_.moveController.getCurrentDir()), VECTOR_UP), VECTOR_UP);

}

void RotatingMoveState::applyCurrentRotation()
{
    if (context_.moveController.isMoving())
    {
        context_.currentAngle = glm::slerp(context_.currentAngle, context_.targetAngle, context_.progress);
        setCharacterRotation(glm::mat4_cast(context_.currentAngle));
    }
}

}  // namespace Components
}  // namespace GameEngine
