#include "AimWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkAndRotateState::AimWalkAndRotateState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , RotateStateBase(context, context.walkSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
{
}
void AimWalkAndRotateState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
