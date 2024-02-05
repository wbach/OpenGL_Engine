#include "ArmedRunState.h"

#include "../CharacterController.h"
#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{
ArmedRunState::ArmedRunState(FsmContext &context)
    : MoveStateBase{context, std::nullopt, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
{
}
void ArmedRunState::update(const AimStopEvent &)
{
    if (drawArrowEndSub_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*drawArrowEndSub_);
        drawArrowEndSub_ = std::nullopt;
    }
}

void ArmedRunState::onLeave()
{
    drawArrowEndSub_ = std::nullopt;
}

}  // namespace Components
}  // namespace GameEngine
