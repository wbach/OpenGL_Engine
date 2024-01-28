#include "AimStateBase.h"

#include <GLM/GLMUtils.h>

#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCameraComponent.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const float defaultCamRotationSpeed = 0.2f;
}
AimStateBase::AimStateBase(FsmContext &context)
    : context_{context}
{
}

void AimStateBase::onEnter()
{
    auto thridPersoncamera = context_.gameObject.GetComponent<ThridPersonCameraComponent>();
    if (thridPersoncamera and thridPersoncamera->thirdPersonCamera)
        thridPersoncamera->thirdPersonCamera->LockInputs(true);
}

void AimStateBase::onEnter(const AimStartEvent &)
{


    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();
}

void AimStateBase::update(float deltaTime)
{
    auto maybeJoint = context_.animator.GetJoint("mixamorig:Spine1");
    if (maybeJoint)
    {
        // rotation.value_ = rotation.value_ * glm::angleAxis(glm::radians(DEFAULT_TURN_SPEED * deltaTime),
        // glm::vec3(0.f, 1.f, 0.f));
        auto v         = context_.inputManager.CalcualteMouseMove();
        auto mouseMove = vec2(v.x, v.y) * defaultCamRotationSpeed;
        yaw += mouseMove.x;
        pitch += mouseMove.y;
        // IncreaseYaw(mouseMove.x);
        // IncreasePitch(mouseMove.y);
        // y += DEFAULT_TURN_SPEED * deltaTime;
        rotation = Rotation(DegreesVec3(0, yaw, pitch));

        maybeJoint->additionalUserMofiyTransform = glm::toMat4(rotation.value_);
        DEBUG_LOG(std::to_string(maybeJoint->additionalUserMofiyTransform));
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
    auto thridPersoncamera = context_.gameObject.GetComponent<ThridPersonCameraComponent>();
    if (thridPersoncamera and thridPersoncamera->thirdPersonCamera)
        thridPersoncamera->thirdPersonCamera->LockInputs(false);
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
void AimStateBase::IncreaseYaw(float yaw)
{
    rotation.value_ *= glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
}
void AimStateBase::IncreasePitch(float pitch)
{
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    rotation.value_  = glm::normalize(qPitch * rotation.value_);
}

}  // namespace Components
}  // namespace GameEngine
