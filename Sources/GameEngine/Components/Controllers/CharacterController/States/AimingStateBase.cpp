#include "AimingStateBase.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimingStateBase::AimingStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName,
                                 const std::string &animationClip)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , animationClip{animationClip}
{
}

void AimingStateBase::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void AimingStateBase::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void AimingStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void AimingStateBase::setAnim()
{
    context_.animator.ChangeAnimation(animationClip, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                      jointGroupName_);
}

void AimingStateBase::stopAnim()
{
    context_.animator.StopAnimation(jointGroupName_);

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }

    context_.aimController.reset();
}

void AimingStateBase::unsubscribeAll()
{
    for (auto id : animationSubIds_)
    {
        context_.animator.UnSubscribeForAnimationFrame(id);
    }
}

void AimingStateBase::update(float)
{
    context_.aimController.update();
}

void AimingStateBase::onLeave(const AimStopEvent &)
{
    stopAnim();

    if (context_.aimEnteringState == FsmContext::AimEnteringState::Run or
        context_.aimEnteringState == FsmContext::AimEnteringState::Sprint)
    {
        context_.characterController.pushEventToQueue(WalkChangeStateEvent{});
    }
}

void AimingStateBase::onLeave(const WeaponStateEvent &)
{
    stopAnim();
}

void AimingStateBase::onLeave(const SprintStateChangeEvent &)
{
    stopAnim();
}

void AimingStateBase::onLeave()
{
    unsubscribeAll();
}

}  // namespace Components
}  // namespace GameEngine
