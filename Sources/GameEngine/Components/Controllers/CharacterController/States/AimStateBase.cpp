#include "AimStateBase.h"

#include <GLM/GLMUtils.h>

#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/Fsm/ThridPersonCameraEvents.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "Input/InputManager.h"
#include "Logger/Log.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"


namespace GameEngine
{
namespace Components
{

AimStateBase::AimStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}

{
}

void AimStateBase::onEnter(const AimStartEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();
}

void AimStateBase::update(float)
{
    context_.aimController.update();
}

void AimStateBase::setAnim()
{
    context_.animator.ChangeAnimation(context_.animClipNames.aimIdle, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}

void AimStateBase::stopAnim()
{
    context_.animator.StopAnimation(jointGroupName_);

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }
    context_.aimController.reset();
}

void AimStateBase::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void AimStateBase::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void AimStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void AimStateBase::onLeave(const AimStopEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onLeave(AimStopEvent)");
    stopAnim();

    if (context_.aimEnteringState == FsmContext::AimEnteringState::Run or
        context_.aimEnteringState == FsmContext::AimEnteringState::Sprint)
    {
        context_.characterController.pushEventToQueue(WalkChangeStateEvent{});
    }
}

void AimStateBase::onLeave(const WeaponStateEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onLeave(WeaponStateEvent)");
    stopAnim();
}

void AimStateBase::onLeave(const SprintStateChangeEvent &)
{
    // /*DISABLED*/ DEBUG_LOG("onLeave(WeaponStateEvent)");
    stopAnim();
}
}  // namespace Components
}  // namespace GameEngine
