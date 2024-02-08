#include "ArmedRotateState.h"

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
ArmedRotateState::ArmedRotateState(FsmContext &context)
    : RotateStateBase{context, std::nullopt, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight}
{
}

void ArmedRotateState::update(const AimStopEvent &)
{
    if (drawArrowEndSub_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*drawArrowEndSub_);
        drawArrowEndSub_ = std::nullopt;
    }
}

void ArmedRotateState::onLeave()
{
    drawArrowEndSub_ = std::nullopt;
}
}  // namespace Components
}  // namespace GameEngine
