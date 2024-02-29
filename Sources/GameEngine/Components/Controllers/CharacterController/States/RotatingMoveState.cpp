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
    //    context_.moveStateData_.moveDirection     = vec3(0.f, 0.f, 1.f);
    //    context_.moveStateData_.currentMoveSpeed_.z = fabsf(moveSpeed_.forward);

    //    setForwardAnim();
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::onEnter(const RunBackwardEvent &event)
{
    //    context_.moveStateData_.moveDirection     = vec3(0.f, 0.f, -1.f);
    //    context_.moveStateData_.currentMoveSpeed_.z = fabsf(moveSpeed_.forward);

    //    setForwardAnim();
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::onEnter(const RunLeftEvent &event)
{
    //    context_.moveStateData_.moveDirection     = vec3(1.f, 0.f, 0.f);
    //    context_.moveStateData_.currentMoveSpeed_.x = fabsf(moveSpeed_.forward);

    MoveStateBase::onEnter(event);
}

void RotatingMoveState::onEnter(const RunRightEvent &event)
{
    //    context_.moveStateData_.moveDirection     = vec3(-1.f, 0.f, 0.f);
    //    context_.moveStateData_.currentMoveSpeed_.x = fabsf(moveSpeed_.forward);

    //    setForwardAnim();
    MoveStateBase::onEnter(event);
}

void RotatingMoveState::update(float)
{
    moveRigidbody();
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
    applyCurrentRotation();
    setAnim(animationClips_.forward);
}

void RotatingMoveState::onLeave()
{
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

float RotatingMoveState::getCurrentAngle() const
{
    return glm::orientedAngle(VECTOR_FORWARD, glm::normalize(context_.moveController.getCurrentDir()), VECTOR_UP);
}

void RotatingMoveState::applyCurrentRotation()
{
    setCharacterRotation(glm::rotate(mat4(1.0f), getCurrentAngle(), glm::vec3(0.f, 1.f, 0.f)));
}

}  // namespace Components
}  // namespace GameEngine
