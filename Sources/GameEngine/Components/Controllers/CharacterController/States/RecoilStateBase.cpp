#include "RecoilStateBase.h"

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilStateBase::RecoilStateBase(FsmContext &contex)
    : context_{contex}
    , animName_{context_.animClipNames.recoilArrow}
    , thridPersonCameraComponent_{contex.gameObject.GetComponent<ThridPersonCameraComponent>()}
{
    context_.animator.setPlayOnceForAnimationClip(animName_);
}

void RecoilStateBase::onEnter(const EndRotationEvent &)
{
    stopMultiAnimation();
}

void RecoilStateBase::onEnter(const EndForwardMoveEvent &)
{
    stopMultiAnimation();
}

void RecoilStateBase::onEnter(const EndBackwardMoveEvent &)
{
    stopMultiAnimation();
}
void RecoilStateBase::onEnter(const AttackEvent &)
{
    DEBUG_LOG("On enter DrawArrowEvent clip: " + animName_);

    if (animName_.empty())
    {
        return;
    }

    setAnim();

    context_.animator.SubscribeForAnimationFrame(
        animName_, [&]() { context_.characterController.fsm()->handle(ReloadArrowEvent{}); });
}
void RecoilStateBase::update(float)
{
    context_.aimController.update();
}

void RecoilStateBase::stopAnim()
{
    context_.multiAnimations = false;
    context_.animator.StopAnimation(context_.upperBodyGroupName);

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }

    context_.aimController.reset();
}

void RecoilStateBase::onLeave(const AimStopEvent &)
{
    stopAnim();
}

void RecoilStateBase::onLeave(const WeaponStateEvent &)
{
    stopAnim();
}

void RecoilStateBase::onLeave(const SprintStartEvent &)
{
    stopAnim();
}

void RecoilStateBase::onLeave(const SprintStateChangeEvent &)
{
    stopAnim();
}
void RecoilStateBase::setAnim()
{
    context_.animator.ChangeAnimation(
        animName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}

void RecoilStateBase::stopMultiAnimation()
{
    context_.multiAnimations = false;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
