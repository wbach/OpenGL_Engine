#include "DisarmedIdleCrouchState.h"

#include "../FsmContext.h"


namespace GameEngine
{
namespace Components
{
DisarmedIdleCrouchState::DisarmedIdleCrouchState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.disarmed.posture.crouched.idle)
{
}
}  // namespace Components
}  // namespace GameEngine
