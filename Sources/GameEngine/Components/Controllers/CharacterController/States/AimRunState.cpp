#include "AimRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRunState::AimRunState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
{
}
void AimRunState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
