#include "RotatingMoveState.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const mat4 matrixRotationForward(1.f);
const mat4 matrixRotationBackward(glm::rotate(mat4(1.0f), ToRadians(180.f), glm::vec3(0.f, 1.f, 0.f)));
const mat4 matrixRotationRight(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
const mat4 matrixRotationLeft(glm::rotate(mat4(1.0f), ToRadians(90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

RotatingMoveState::RotatingMoveState(FsmContext &context, const std::optional<std::string> &jointGroup, float moveSpeed,
                                     const std::string &forwardAnimName)
    : MoveStateBase(context, jointGroup, moveSpeed, forwardAnimName)
{
}

void RotatingMoveState::onEnter(const RunForwardEvent &)
{
    context_.moveStateData_.moveDirection     = vec3(0.f, 0.f, 1.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);

    setForwardAnim();
    setCharacterRotation(matrixRotationForward);
}

void RotatingMoveState::onEnter(const RunBackwardEvent &)
{
    context_.moveStateData_.moveDirection     = vec3(0.f, 0.f, -1.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);

    setForwardAnim();
    setCharacterRotation(matrixRotationBackward);
}

void RotatingMoveState::onEnter(const RunLeftEvent &)
{
    context_.moveStateData_.moveDirection     = vec3(1.f, 0.f, 0.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);

    setForwardAnim();
    setCharacterRotation(matrixRotationLeft);
}

void RotatingMoveState::onEnter(const RunRightEvent &)
{
    context_.moveStateData_.moveDirection     = vec3(-1.f, 0.f, 0.f);
    context_.moveStateData_.currentMoveSpeed_ = fabsf(moveSpeed_.forward);

    setForwardAnim();
    setCharacterRotation(matrixRotationRight);
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

void RotatingMoveState::setCharacterRotation(const mat4 &matrixRotation)
{
    auto rj = context_.animator.GetRootJoint();
    if (rj)
    {
        rj->additionalUserMofiyTransform.set(matrixRotation);
    }
}

}  // namespace Components
}  // namespace GameEngine
