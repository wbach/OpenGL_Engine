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
const vec3 xVector(1.f, 0.f, 0.f);
const vec3 zVector(0.f, 0.f, 1.f);
const mat4 matrixRotationOffset(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

AimStateBase::AimStateBase(FsmContext &context, const std::string &jointName)
    : context_{context}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , joint_{context_.animator.GetJoint(jointName)}
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
        if (joint_)
        {
            thridPersonCameraComponent_->handleEvent(Camera::StartAimEvent{joint_->id});
        }
    }
    else
    {
        DEBUG_LOG("ThridPersonCameraComponent Component not found! Try again...");
    }

    if (joint_)
    {
        joint_->ignoreParentRotation         = true;
        joint_->additionalUserMofiyTransform = matrixRotationOffset;

        calculateMouseMove();  // skip first random move
    }
}

void AimStateBase::update(float deltaTime)
{
    if (joint_)
    {
        auto mouseMove = calculateMouseMove();

        IncreaseYRotation(joint_->additionalRotations.y, -mouseMove.x);
        IncreaseXZRotation(joint_->additionalRotations.z, -mouseMove.y, zVector);
        IncreaseXZRotation(joint_->additionalRotations.x, mouseMove.y, xVector);

        joint_->additionalUserMofiyTransform =
            matrixRotationOffset *
            glm::mat4_cast(joint_->additionalRotations.y.value_ * joint_->additionalRotations.z.value_);
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

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }

    if (joint_)
    {
        // reset joint position
        joint_->additionalRotations          = Animation::Joint::AdditionalRotations{};
        joint_->additionalUserMofiyTransform = mat4(1.f);
        joint_->ignoreParentRotation         = false;
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
void AimStateBase::IncreaseYRotation(Rotation &rotation, float value)
{
    rotation.value_ *= glm::normalize(glm::angleAxis(glm::radians(value), glm::vec3(0.f, 1.f, 0.f)));
}
void AimStateBase::IncreaseXZRotation(Rotation &rotation, float value, const vec3 &dir)
{
    glm::quat qPitch = glm::angleAxis(glm::radians(value), dir);
    rotation.value_  = glm::normalize(qPitch * rotation.value_);
}

}  // namespace Components
}  // namespace GameEngine
