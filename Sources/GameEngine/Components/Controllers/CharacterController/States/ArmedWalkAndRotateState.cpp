#include "ArmedWalkAndRotateState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedWalkAndRotateState::ArmedWalkAndRotateState(FsmContext& context)
    : RotatingMoveAndRotateStateBase{context,
                                     std::nullopt,
                                     context.walkSpeed,
                                     context.animClipNames.armed.walk,
                                     context.animClipNames.armed.rotateLeft,
                                     context.animClipNames.armed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
