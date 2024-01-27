#include "DisarmedIdleState.h"
#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
DisarmedIdleState::DisarmedIdleState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.disarmed.idle)
{
}

void DisarmedIdleState::onEnter(const WeaponStateEvent &)
{
    StateBase::disarmWeapon();
}

void DisarmedIdleState::onLeave(const DrawArrowEvent &)
{
    drawArrowEndSub_ = context_.animator.SubscribeForAnimationFrame(context_.animClipNames.equip, [&]()
    {
        context_.characterController.fsm()->handle(DrawArrowEvent{});
    });
}

}  // namespace Components
}  // namespace GameEngine
