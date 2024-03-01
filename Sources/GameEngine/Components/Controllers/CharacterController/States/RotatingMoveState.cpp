#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
RotatingMoveState::RotatingMoveState(FsmContext &context, const std::optional<std::string> &jointGroup, float moveSpeed,
                                     const std::string &forwardAnimName)
    : MoveStateBase(context, jointGroup, moveSpeed, forwardAnimName)
    , targetAngle(vec3(0.f))
    , currentAngle(vec3(0.f))
    , sourceAngle(vec3(0.f))
    , progress{1.f}
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

    if (progress < 1.f)
    {
        const float rotateTime{0.5f};
        progress += dt/rotateTime;
        applyCurrentRotation();
    }
    else
    {
        progress = 1.f;
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
    progress = 0.f;
    sourceAngle = currentAngle;
    targetAngle = glm::angleAxis(
        glm::orientedAngle(VECTOR_FORWARD, glm::normalize(context_.moveController.getCurrentDir()), VECTOR_UP), VECTOR_UP);

}

void RotatingMoveState::applyCurrentRotation()
{
    if (context_.moveController.isMoving())
    {
        currentAngle = glm::slerp(currentAngle, targetAngle, progress);
        setCharacterRotation(glm::mat4_cast(currentAngle));
    }
}

}  // namespace Components
}  // namespace GameEngine
