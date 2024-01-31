#include "AimStateBase.h"

#include <GLM/GLMUtils.h>

#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const mat4 matrixRotationOffset(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

AimStateBase::AimStateBase(FsmContext &context)
    : context_{context}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , camSensitive{0.2f}
    , yawLimit{-75.f, 45.f}
    , pitchLimit{-40.f, 50.f}
{
}

void AimStateBase::onEnter()
{
}

void AimStateBase::onEnter(const AimStartEvent &)
{
    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();

    if (context_.aimingJoint)
    {
        context_.aimingJoint->ignoreParentRotation         = true;
        context_.aimingJoint->additionalUserMofiyTransform = matrixRotationOffset;
    }

    context_.aimYaw   = 0.f;
    context_.aimPitch = 0.f;
    context_.inputManager.CalcualteMouseMove();
}

void AimStateBase::update(float)
{
    if (context_.aimingJoint)
    {
        auto mouseMove = calculateMouseMove();
        context_.aimYaw -= mouseMove.x;
        context_.aimPitch -= mouseMove.y;

        LockYaw();
        LockPitch();

        context_.aimingJoint->additionalRotations.x.value_ =
            glm::normalize(glm::angleAxis(glm::radians(-context_.aimPitch), glm::vec3(1.f, 0.f, 0.f)));
        context_.aimingJoint->additionalRotations.y.value_ =
            glm::normalize(glm::angleAxis(glm::radians(context_.aimYaw), glm::vec3(0.f, 1.f, 0.f)));
        context_.aimingJoint->additionalRotations.z.value_ =
            glm::normalize(glm::angleAxis(glm::radians(context_.aimPitch), glm::vec3(0.f, 0.f, 1.f)));

        context_.aimingJoint->additionalUserMofiyTransform =
            matrixRotationOffset * glm::mat4_cast(context_.aimingJoint->additionalRotations.y.value_ *
                                                  context_.aimingJoint->additionalRotations.z.value_);
    }
}

void AimStateBase::setAnim()
{
    context_.animator.ChangeAnimation(
        context_.animClipNames.aimIdle, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}

void AimStateBase::stopAnim()
{
    context_.multiAnimations = false;
    context_.animator.StopAnimation(context_.upperBodyGroupName);

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }

    if (context_.aimingJoint)
    {
        // reset joint position
        context_.aimingJoint->additionalRotations          = Animation::Joint::AdditionalRotations{};
        context_.aimingJoint->additionalUserMofiyTransform = mat4(1.f);
        context_.aimingJoint->ignoreParentRotation         = false;
    }
}

vec2 AimStateBase::calculateMouseMove()
{
    auto mouseMove = context_.inputManager.CalcualteMouseMove();
    return vec2(static_cast<float>(mouseMove.x), static_cast<float>(mouseMove.y)) * camSensitive;
}

void AimStateBase::onEnter(const EndRotationEvent &)
{
    stopMultiAnimation();
}

void AimStateBase::onEnter(const EndForwardMoveEvent &)
{
    stopMultiAnimation();
}

void AimStateBase::onEnter(const EndBackwardMoveEvent &)
{
    stopMultiAnimation();
}

void AimStateBase::onLeave(const AimStopEvent &)
{
    DEBUG_LOG("onLeave(AimStopEvent)");
    stopAnim();
}

void AimStateBase::onLeave()
{
}

void AimStateBase::onLeave(const WeaponStateEvent &)
{
    DEBUG_LOG("onLeave(WeaponStateEvent)");
    stopAnim();
}
void AimStateBase::stopMultiAnimation()
{
    context_.multiAnimations = false;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void AimStateBase::IncreaseYRotation(Rotation &rotation, float value)
{
    rotation.value_ *= glm::normalize(glm::angleAxis(glm::radians(value), glm::vec3(0.f, 1.f, 0.f)));
}
void AimStateBase::IncreaseXZRotation(Rotation &rotation, float value, const vec3 &dir)
{
    glm::quat qPitch = glm::angleAxis(glm::radians(value), dir);
    rotation.value_  = glm::normalize(qPitch * rotation.value_);
}
void AimStateBase::LockPitch()
{
    if (context_.aimPitch < pitchLimit.x)
        context_.aimPitch = pitchLimit.x;
    if (context_.aimPitch > pitchLimit.y)
        context_.aimPitch = pitchLimit.y;
}

void AimStateBase::LockYaw()
{
    if (context_.aimYaw < yawLimit.x)
        context_.aimYaw = yawLimit.x;
    if (context_.aimYaw > yawLimit.y)
        context_.aimYaw = yawLimit.y;
}
}  // namespace Components
}  // namespace GameEngine
