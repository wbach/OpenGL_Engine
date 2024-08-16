#include "ArmedWalkAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedWalkAndRotateState::ArmedWalkAndRotateState(FsmContext& context)
    : RotatingMoveAndRotateStateBase{context, std::nullopt, context.walkSpeed, context.animClipNames.armed.movement.walk,
                                     context.animClipNames.armed.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
