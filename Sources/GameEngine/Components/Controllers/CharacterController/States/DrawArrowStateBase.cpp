#include "DrawArrowStateBase.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowStateBase::DrawArrowStateBase(FsmContext& context)
    : context_{context}
{
}

void DrawArrowStateBase::onEnter(const DrawArrowEvent&)
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

    subId_ = context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.drawArrow, [&]() { context_.characterController.fsm()->handle(AimStartEvent{}); });
}

void DrawArrowStateBase::onEnter(const EndRotationEvent&)
{
    context_.multiAnimations = false;
    DEBUG_LOG("onEnter EndRotationEvent clip : " + context_.animClipNames.drawArrow);
    setAnim();
}

void DrawArrowStateBase::setAnim()
{
    context_.animator.ChangeAnimation(
        context_.animClipNames.drawArrow, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}

void DrawArrowStateBase::update(float)
{
}

}  // namespace Components
}  // namespace GameEngine
