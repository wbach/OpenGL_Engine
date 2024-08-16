#include "DisarmedWalkAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedWalkAndRotateState::DisarmedWalkAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context, std::nullopt, context.walkSpeed, context.animClipNames.disarmed.movement.walk,
                                     context.animClipNames.disarmed.posture.stand.rotate}
{
}
}  // namespace Components
}  // namespace GameEngine
