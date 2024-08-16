#include "ArmedIdleCrouchState.h"

#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleCrouchState::ArmedIdleCrouchState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.armed.posture.crouched.idle)
{
}
}  // namespace Components
}  // namespace GameEngine
