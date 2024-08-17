#include "ArmedCrouchWalkAndRotateState.h"

#include "../FsmContext.h"


namespace GameEngine
{
namespace Components
{
ArmedCrouchWalkAndRotateState::ArmedCrouchWalkAndRotateState(FsmContext& context)
    : RotatingMoveAndRotateStateBase{context, std::nullopt, context.walkSpeed, context.animClipNames.armed.movement.crouch,
                                     context.animClipNames.armed.posture.crouched.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
