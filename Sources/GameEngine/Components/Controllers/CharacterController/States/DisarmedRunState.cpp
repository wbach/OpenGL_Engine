#include "DisarmedRunState.h"
#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{

DisarmedRunState::DisarmedRunState(FsmContext &context)
    : MoveStateBase{context, context.runSpeed, context.animClipNames.disarmed.run.forward,
                    context.animClipNames.disarmed.run.backward}
{
}

void DisarmedRunState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    StateBase::disarmWeapon();
    MoveStateBase::setCurrentAnim();
}

void DisarmedRunState::onLeave(const DrawArrowEvent &)
{
    drawArrowEndSub_ = context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.equip, [&]() { context_.characterController.fsm()->handle(DrawArrowEvent{}); });
}

}  // namespace Components
}  // namespace GameEngine
