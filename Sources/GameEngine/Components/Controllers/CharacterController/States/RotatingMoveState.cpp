#include "RotatingMoveState.h"

#include <GLM/GLMUtils.h>

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"

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

void RotatingMoveState::update(float dt)
{
    // /*DISABLED*/ DEBUG_LOG("Update");
    moveCameraRotateRigidbody();

    if (context_.progress < 1.f)
    {
        context_.progress += dt / DEFAULT_MOVING_CHANGE_DIR_SPEED;
        applyCurrentRotation();
    }
}

void RotatingMoveState::update(const MoveForwardEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::update(const MoveBackwardEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::update(const MoveLeftEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::update(const MoveRightEvent &event)
{
    onEnter(event);
}

void RotatingMoveState::postEnter()
{
    // /*DISABLED*/ DEBUG_LOG("postEnter");
    // prevent to call postEnter from MoveStateBase
    setTargetAngle();
    setAnim(animationClips_.forward);
}

void RotatingMoveState::postUpdate()
{
    // /*DISABLED*/ DEBUG_LOG("postUpdate");
    setTargetAngle();
    if (not context_.moveController.isMoving())
    {
        // /*DISABLED*/ DEBUG_LOG("onMoveInactivity");
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
        // /*DISABLED*/ DEBUG_LOG("not setCharacterRotation");
    }
}

void RotatingMoveState::setTargetAngle()
{
    // /*DISABLED*/ DEBUG_LOG("Current dir : " + std::to_string(context_.moveController.getCurrentDir()));
    context_.progress    = 0.f;
    context_.sourceAngle = context_.currentAngle;
    context_.targetAngle = glm::angleAxis(
        glm::orientedAngle(VECTOR_FORWARD, glm::normalize(context_.moveController.getCurrentDir()), VECTOR_UP),
        VECTOR_UP);
}

void RotatingMoveState::applyCurrentRotation()
{
    if (context_.moveController.isMoving())
    {
        // /*DISABLED*/ DEBUG_LOG("Progress " + std::to_string(context_.progress));
        auto progress         = context_.progress > 1.f ? 1.f : context_.progress;
        // /*DISABLED*/ DEBUG_LOG("Progress2 " + std::to_string(progress));
        context_.currentAngle = glm::slerp(context_.currentAngle, context_.targetAngle, progress);
        setCharacterRotation(glm::mat4_cast(context_.currentAngle));

        if (context_.progress >= 1.f)
        {
            context_.progress = 2.f;
        }
    }
}

void RotatingMoveState::moveCameraRotateRigidbody()  // TO DO: refactor with moveStateBase
{
    if (not context_.moveController.isMoving())
    {
        // // /*DISABLED*/ DEBUG_LOG("Not moving, return");
        return;
    }

    auto moveDirection = glm::normalize(context_.moveController.getCurrentDir());
    auto moveSpeed =
        glm::length(vec3(moveSpeed_.leftRight, 0, moveDirection.z > 0.5f ? moveSpeed_.forward : moveSpeed_.backward) *
                    moveDirection);

    if (moveSpeed < 0.00001f)
    {
        return;
    }

    if (auto tcc = context_.gameObject.GetComponent<ThridPersonCameraComponent>())
    {
        auto [_, yaw] = tcc->getRotation();
        auto rotY     = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
        context_.rigidbody.SetRotation(rotY);
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
