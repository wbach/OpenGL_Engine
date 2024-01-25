#include "DrawArrowState.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowState::DrawArrowState(FsmContext &context)
    : context_{context}
{
    context_.animator.setPlayOnceForAnimationClip(context_.animClipNames.drawArrow);
}

void DrawArrowState::onEnter(const DrawArrowEvent &)
{
    DEBUG_LOG("On enter DrawArrowEvent clip: " + context_.animClipNames.drawArrow);

    if (context_.animClipNames.drawArrow.empty())
    {
        return;
    }

    setAnim();

    context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.drawArrow, [&]() { context_.characterController.fsm()->handle(AimStartEvent{}); });
}

void DrawArrowState::setAnim()
{
    context_.animator.ChangeAnimation(
        context_.animClipNames.drawArrow, Animator::AnimationChangeType::smooth, PlayDirection::forward,
        context_.multiAnimations ? std::make_optional(context_.upperBodyGroupName) : std::nullopt);
}

}  // namespace Components
}  // namespace GameEngine
