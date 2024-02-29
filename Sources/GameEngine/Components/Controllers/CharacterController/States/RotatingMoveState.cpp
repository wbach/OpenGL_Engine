#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
RotatingMoveState::RotatingMoveState(FsmContext &context, const std::optional<std::string> &jointGroup, float moveSpeed,
                                     const std::string &forwardAnimName)
    : MoveStateBase(context, jointGroup, moveSpeed, forwardAnimName)
    , targetAngle{0.f}
    , currentAngle{0.f}
    , rotateSpeed{10.f}
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
    if (not compare(currentAngle, targetAngle, 0.1f))
    {
        if (targetAngle > currentAngle)
        {
            currentAngle += dt * rotateSpeed;
        }
        else
        {
            currentAngle -= dt * rotateSpeed;
        }

        applyCurrentRotation();
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
    targetAngle =
        glm::orientedAngle(VECTOR_FORWARD, glm::normalize(context_.moveController.getCurrentDir()), VECTOR_UP);

    DEBUG_LOG("targetAngle degrees : " + std::to_string(glm::degrees(targetAngle)));
    DEBUG_LOG("targetAngle radians : " + std::to_string((targetAngle)));
}

void RotatingMoveState::applyCurrentRotation()
{
    if (context_.moveController.isMoving())
    {
        if (currentAngle < -180.f)
        {
            currentAngle += 180.f;
        }
        if (currentAngle > 180.f)
        {
            currentAngle -= 180.f;
        }
        setCharacterRotation(glm::rotate(mat4(1.0f), currentAngle, glm::vec3(0.f, 1.f, 0.f)));
    }
}

}  // namespace Components
}  // namespace GameEngine
