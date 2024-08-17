#include "DisarmedCrouchWalkAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedCrouchWalkAndRotateState::DisarmedCrouchWalkAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context, std::nullopt, context.walkSpeed, context.animClipNames.disarmed.movement.crouch,
                                     context.animClipNames.disarmed.posture.crouched.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
