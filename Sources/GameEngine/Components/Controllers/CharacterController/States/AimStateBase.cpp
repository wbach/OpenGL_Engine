#include "AimStateBase.h"

namespace GameEngine
{
namespace Components
{
AimStateBase::AimStateBase(FsmContext &context)
    : context_{context}
{
}

void AimStateBase::onEnter(const AimStartEvent &)
{
    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    setAnim();
}

void AimStateBase::update(float)
{
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
}  // namespace Components
}  // namespace GameEngine
