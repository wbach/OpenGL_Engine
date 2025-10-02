#include "DrawArrowStateBase.h"

#include <Logger/Log.h>

#include "../FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/AimController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"

namespace GameEngine
{
namespace Components
{
DrawArrowStateBase::DrawArrowStateBase(FsmContext &context, const std::optional<std::string> &jointGroupName)
    : AimingStateBase{context, jointGroupName, context.animClipNames.aim.draw}
{
    context.animator.setPlayOnceForAnimationClip(animationClip);
}

void DrawArrowStateBase::onEnter(const DrawArrowEvent &)
{
    onEnter(ReloadArrowEvent{});

    if (thridPersonCameraComponent_)
    {
        thridPersonCameraComponent_->handleEvent(Camera::StartAimEvent{context_.aimController.getJoint().id});
    }

    auto rj = context_.animator.GetRootJoint();
    if (rj)
    {
        rj->additionalUserMofiyTransform.set(mat4{1.f});
    }

    context_.aimController.enter();
}

void DrawArrowStateBase::onEnter(const ReloadArrowEvent &)
{
    if (context_.animClipNames.aim.draw.empty())
    {
        return;
    }

    setAnim();

    unsubscribeAll();

    context_.aimController.reload();
    animationSubIds_.push_back(context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.aim.draw, [&]() { context_.characterController.pushEventToQueue(AimStartEvent{}); }));
}
}  // namespace Components
}  // namespace GameEngine
