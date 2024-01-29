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
AimStateBase::AimStateBase(FsmContext &context)
    : context_{context}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , rotation(DegreesVec3(0.f, 0.f, 0.f))
{
}

void AimStateBase::onEnter()
{
}

void AimStateBase::onEnter(const AimStartEvent &)
{
    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StartAimEvent{});
    }
    else
    {
        DEBUG_LOG("ThridPersonCameraComponent Component not found! Try again...");
        thridPersonCameraComponent_ = context_.gameObject.GetComponent<ThridPersonCameraComponent>();
        if (thridPersonCameraComponent_)
        {
            thridPersonCameraComponent_->handleEvent(Camera::StartAimEvent{});
        }
    }
}

void AimStateBase::update(float deltaTime)
{
    // return;

    auto maybeJoint = context_.animator.GetJoint("mixamorig:Spine1");
    if (maybeJoint)
    {
        // rotation.value_ = rotation.value_ * glm::angleAxis(glm::radians(DEFAULT_TURN_SPEED * deltaTime),
        // glm::vec3(0.f, 1.f, 0.f));
        auto mouseMove = context_.inputManager.CalcualteMouseMove();
        IncreaseYaw(-static_cast<float>(mouseMove.x) * camSensitive);
        IncreasePitch(static_cast<float>(mouseMove.y) * camSensitive);

        // auto mouseMove = vec2(-v.x, v.y) * defaultCamRotationSpeed;
        // yaw += mouseMove.x;
        // pitch += mouseMove.y;
        //  IncreaseYaw(mouseMove.x);
        //  IncreasePitch(mouseMove.y);
        //  y += DEFAULT_TURN_SPEED * deltaTime;
        // rotation = Rotation(DegreesVec3(0, yaw, 0 /*pitch*/));

        maybeJoint->additionalUserMofiyTransform = glm::mat4_cast(rotation.value_);
        // DEBUG_LOG(std::to_string(maybeJoint->additionalUserMofiyTransform));
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

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }
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
void AimStateBase::IncreaseYaw(float yaw)
{
    rotation.value_ *= glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
    DEBUG_LOG("rotation.value_ = " + std::to_string(rotation.value_));
}
void AimStateBase::IncreasePitch(float pitch)
{
    DEBUG_LOG("rotation.value_ = " + std::to_string(rotation.value_));
    //    rotation.value_*= glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    //    return;
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
    rotation.value_  = glm::normalize(qPitch * rotation.value_);

    //    auto euler = glm::eulerAngles(rotation.value_).y;

    //   // auto rotY = context_.gameObject.GetWorldTransform().GetPosition().y;
    //    float x   = glm::radians(pitch) * glm::sin(euler);
    //    float y   = glm::radians(pitch) * glm::cos(euler);

    //    glm::quat qPitch = glm::angleAxis(x, glm::vec3(0, 0, 1));
    //    rotation.value_  = glm::normalize(qPitch * rotation.value_);

    //    glm::quat qRoll          = glm::angleAxis(y, glm::vec3(1, 0, 0));
    //    rotation.value_ = glm::normalize(qRoll * rotation.value_);
}

}  // namespace Components
}  // namespace GameEngine
