#include "AimRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRunAndRotateState::AimRunAndRotateState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
{
}
void AimRunAndRotateState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
