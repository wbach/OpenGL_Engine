#include "ArmedWalkAndRotateState.h"

namespace GameEngine
{
namespace Components
{
ArmedWalkAndRotateState::ArmedWalkAndRotateState(FsmContext& context)
    : MoveAndRotateStateBase{context,
                             std::nullopt,
                             context.walkSpeed,
                             context.animClipNames.armed.walk,
                             context.animClipNames.armed.rotateLeft,
                             context.animClipNames.armed.rotateRight}
    , context_{context}
{
}
}  // namespace Components
}  // namespace GameEngine