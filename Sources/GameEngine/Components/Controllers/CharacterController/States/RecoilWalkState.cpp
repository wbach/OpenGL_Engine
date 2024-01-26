#include "RecoilWalkState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilWalkState::RecoilWalkState(FsmContext& context)
    : RecoilStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
{
}
void RecoilWalkState::update(float dt)
{
    MoveStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
