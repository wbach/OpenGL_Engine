#include "RecoilRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilRunState::RecoilRunState(FsmContext& context)
    : RecoilStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
{
}
void RecoilRunState::update(float dt)
{
    MoveStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
