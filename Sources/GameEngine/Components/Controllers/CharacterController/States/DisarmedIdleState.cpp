#include "DisarmedIdleState.h"

#include "../CharacterController.h"
#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Components/Controllers/CharacterController/States/BaseState.h"

namespace GameEngine
{
namespace Components
{
DisarmedIdleState::DisarmedIdleState(FsmContext &context)
    : BaseState(context)
    , IdleStateBase(context, context.animClipNames.disarmed.posture.stand.idle)
{
    setIdleAnim();
}
void DisarmedIdleState::onEnter(const StartDialogEvent &e)
{
    context_.characterController.pushEventToQueue(e);
}
}  // namespace Components
}  // namespace GameEngine
