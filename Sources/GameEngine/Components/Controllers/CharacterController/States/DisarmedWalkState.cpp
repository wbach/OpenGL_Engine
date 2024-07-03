#include "DisarmedWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedWalkState::DisarmedWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.disarmed.walk.forward}
{
}
void DisarmedWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.disarmed.idle);
}
}  // namespace Components
}  // namespace GameEngine
