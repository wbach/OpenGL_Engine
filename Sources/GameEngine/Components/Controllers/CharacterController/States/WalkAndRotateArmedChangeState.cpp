#include "WalkAndRotateArmedChangeState.h"

namespace GameEngine
{
namespace Components
{
WalkAndRotateArmedChangeState::WalkAndRotateArmedChangeState(FsmContext& context)
    : DrawArrowStateBase(context)
    , MoveAndRotateStateBase{context, context.walkSpeed, context.animClipNames.armed.walk,
                             context.animClipNames.armed.rotateLeft, context.animClipNames.armed.rotateRight}
    , context_{context}
{
}

}  // namespace Components
}  // namespace GameEngine
