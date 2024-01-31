#include "DrawArrowStateBase.h"

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowStateBase::DrawArrowStateBase(FsmContext &context)
    : context_{context}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
{
    context.animator.setPlayOnceForAnimationClip(context.animClipNames.drawArrow);
}

void DrawArrowStateBase::onEnter(const DrawArrowEvent &)
{
    DEBUG_LOG("On enter DrawArrowEvent clip: " + context_.animClipNames.drawArrow);

    if (context_.animClipNames.drawArrow.empty())
    {
        return;
    }

    setAnim();

    if (subId_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*subId_);
    }

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StartAimEvent{context_.aimingJoint->id});
    }

    subId_ = context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.drawArrow, [&]() { context_.characterController.fsm()->handle(AimStartEvent{}); });
}

void DrawArrowStateBase::onEnter(const EndRotationEvent &)
{
    stopMultiAnimation();
}

void DrawArrowStateBase::onEnter(const EndForwardMoveEvent &)
{
    stopMultiAnimation();
}

void DrawArrowStateBase::onEnter(const EndBackwardMoveEvent &)
{
    stopMultiAnimation();
}

void DrawArrowStateBase::setAnim()
{
    context_.animator.ChangeAnimation(
        context_.animClipNames.drawArrow, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}

void DrawArrowStateBase::stopAnim()
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

void DrawArrowStateBase::update(float)
{
}

void DrawArrowStateBase::onLeave(const AimStopEvent &)
{
    stopAnim();
}

void DrawArrowStateBase::onLeave(const WeaponStateEvent &)
{
    stopAnim();
}
void DrawArrowStateBase::stopMultiAnimation()
{
    context_.multiAnimations = false;
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
}  // namespace Components
}  // namespace GameEngine
