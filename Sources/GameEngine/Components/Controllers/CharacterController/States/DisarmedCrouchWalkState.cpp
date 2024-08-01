#include "DisarmedCrouchWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedCrouchWalkState::DisarmedCrouchWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.disarmed.crouch.forward}
{
}
void DisarmedCrouchWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.disarmed.crouchIdle);
}
}  // namespace Components
}  // namespace GameEngine
