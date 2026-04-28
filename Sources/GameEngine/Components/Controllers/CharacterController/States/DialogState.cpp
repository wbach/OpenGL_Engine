#include "DialogState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "Logger/Log.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
DialogState::DialogState(FsmContext &context)
    : BaseState(context)
    , IdleStateBase(context, context.animClipNames.disarmed.dialog.say)
{
    setIdleAnim();
}
void DialogState::onEnter(const StartDialogEvent &event)
{
    switch (event.role)
    {
        case StartDialogEvent::Role::Listener:
            idleAnimName_ = context_.animClipNames.disarmed.dialog.listen;
            break;

        case StartDialogEvent::Role::Speaker:
            idleAnimName_ = context_.animClipNames.disarmed.dialog.say;
            break;
        default:
            idleAnimName_ = context_.animClipNames.disarmed.posture.stand.idle;
            break;
    }

    setIdleAnim();
}
void DialogState::update(const StartDialogEvent &event)
{
    onEnter(event);
}
}  // namespace Components
}  // namespace GameEngine
