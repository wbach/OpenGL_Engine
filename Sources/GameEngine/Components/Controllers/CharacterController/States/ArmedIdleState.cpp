#include "ArmedIdleState.h"

#include "../CharacterController.h"
#include "DisarmedIdleState.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleState::ArmedIdleState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.armed.idle)
{
}
}  // namespace Components
}  // namespace GameEngine
