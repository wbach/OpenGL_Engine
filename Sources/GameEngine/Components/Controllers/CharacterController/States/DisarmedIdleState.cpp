#include "DisarmedIdleState.h"
#include "../FsmContext.h"

namespace GameEngine
{
namespace Components
{
DisarmedIdleState::DisarmedIdleState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.disarmed.idle)
{
}
}  // namespace Components
}  // namespace GameEngine
