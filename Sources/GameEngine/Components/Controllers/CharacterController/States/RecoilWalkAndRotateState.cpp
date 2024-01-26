#include "RecoilWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilWalkAndRotateState::RecoilWalkAndRotateState(FsmContext& context)
    : RecoilStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , RotateStateBase(context, context.walkSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
{
}
void RecoilWalkAndRotateState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
