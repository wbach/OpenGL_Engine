#include "ArmedIdleState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleState::ArmedIdleState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.armed.posture.stand.idle)
{
}
}  // namespace Components
}  // namespace GameEngine
