#include "RecoilStateBase.h"

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilStateBase::RecoilStateBase(FsmContext &contex, const std::optional<std::string> &jointGroupName)
    : context_{contex}
    , jointGroupName_{jointGroupName}
    , animName_{context_.animClipNames.recoilArrow}
    , thridPersonCameraComponent_{contex.gameObject.GetComponent<ThridPersonCameraComponent>()}
{
    context_.animator.setPlayOnceForAnimationClip(animName_);
}

void RecoilStateBase::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void RecoilStateBase::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void RecoilStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
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
    context_.animator.StopAnimation(jointGroupName_);

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
    context_.animator.ChangeAnimation(animName_, Animator::AnimationChangeType::smooth, PlayDirection::forward,
                                      jointGroupName_);
}

}  // namespace Components
}  // namespace GameEngine
