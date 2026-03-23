#include "DisarmedIdleState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"

namespace GameEngine
{
namespace Components
{
DisarmedIdleState::DisarmedIdleState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.disarmed.posture.stand.idle)
{
    setIdleAnim();
}
}  // namespace Components
}  // namespace GameEngine
