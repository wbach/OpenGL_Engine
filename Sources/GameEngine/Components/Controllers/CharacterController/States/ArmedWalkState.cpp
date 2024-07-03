#include "ArmedWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedWalkState::ArmedWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.armed.walk.forward}
{
}
void ArmedWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.armed.idle);
}
}  // namespace Components
}  // namespace GameEngine
