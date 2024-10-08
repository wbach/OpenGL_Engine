#include "ArmedCrouchWalkState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedCrouchWalkState::ArmedCrouchWalkState(FsmContext &context)
    : RotatingMoveState{context, std::nullopt, context.walkSpeed.forward, context.animClipNames.armed.movement.crouch.forward}
{
}
void ArmedCrouchWalkState::onMoveInactivity()
{
    setAnim(context_.animClipNames.armed.posture.crouched.idle);
}
}  // namespace Components
}  // namespace GameEngine
