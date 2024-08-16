#include "DisarmedCrouchWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedCrouchWalkState::DisarmedCrouchWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.disarmed.movement.crouch.forward}
{
}
void DisarmedCrouchWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.disarmed.posture.crouched.idle);
}
}  // namespace Components
}  // namespace GameEngine
