#include "DisarmedWalkAndRotateState.h"

namespace GameEngine
{
namespace Components
{
DisarmedWalkAndRotateState::DisarmedWalkAndRotateState(FsmContext &context)
    : RotatingMoveAndRotateStateBase{context,
                                     std::nullopt,
                                     context.walkSpeed,
                                     context.animClipNames.disarmed.walk,
                                     context.animClipNames.disarmed.rotateLeft,
                                     context.animClipNames.disarmed.rotateRight}
{
}
}  // namespace Components
}  // namespace GameEngine
