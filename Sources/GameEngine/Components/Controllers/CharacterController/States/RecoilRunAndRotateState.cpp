#include "RecoilRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilRunAndRotateState::RecoilRunAndRotateState(FsmContext& context)
    : RecoilStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
{
}
void RecoilRunAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
