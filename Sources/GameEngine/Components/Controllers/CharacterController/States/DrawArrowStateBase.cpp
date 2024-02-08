#include "DrawArrowStateBase.h"

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowStateBase::DrawArrowStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : context_{context}
    , jointGroupName_{jointGroupName}
    , thridPersonCameraComponent_{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
{
    context.animator.setPlayOnceForAnimationClip(context.animClipNames.drawArrow);
}

void DrawArrowStateBase::onEnter(const DrawArrowEvent &)
{
    DEBUG_LOG("On enter DrawArrowEvent clip: " + context_.animClipNames.drawArrow);

    onEnter(ReloadArrowEvent{});

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StartAimEvent{context_.aimController.getJoint().id});
    }

    context_.aimController.enter();
}

void DrawArrowStateBase::onEnter(const ReloadArrowEvent &)
{
    if (context_.animClipNames.drawArrow.empty())
    {
        return;
    }

    setAnim();

    if (subId_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*subId_);
    }

    subId_ = context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.drawArrow, [&]() { context_.characterController.pushEventToQueue(AimStartEvent{}); });
}

void DrawArrowStateBase::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void DrawArrowStateBase::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void DrawArrowStateBase::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(jointGroupName_);
}

void DrawArrowStateBase::setAnim()
{
    context_.animator.ChangeAnimation(context_.animClipNames.drawArrow, Animator::AnimationChangeType::smooth,
                                      PlayDirection::forward, jointGroupName_);
}

void DrawArrowStateBase::stopAnim()
{
    context_.animator.StopAnimation(jointGroupName_);

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StopAimEvent{});
    }

    context_.aimController.reset();
}

void DrawArrowStateBase::update(float)
{
    context_.aimController.update();
}

void DrawArrowStateBase::onLeave(const AimStopEvent &)
{
    stopAnim();
}

void DrawArrowStateBase::onLeave(const WeaponStateEvent &)
{
    stopAnim();
}

void DrawArrowStateBase::onLeave(const SprintStartEvent &)
{
    stopAnim();
}

void DrawArrowStateBase::onLeave(const SprintStateChangeEvent &)
{
    stopAnim();
}

}  // namespace Components
}  // namespace GameEngine
