#include "ArmedRotateState.h"
#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
ArmedRotateState::ArmedRotateState(FsmContext &context)
    : RotateStateBase{context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight}
{
}

void ArmedRotateState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    StateBase::equipWeapon();
    RotateStateBase::setCurrentAnim();
}
void ArmedRotateState::onEnter(DisarmedRunState &, const DrawArrowEvent &)
{
    drawArrowEndSub_ = context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.equip, [&]() { context_.characterController.fsm()->handle(DrawArrowEvent{}); });
}

void ArmedRotateState::onEnter(const DrawArrowEvent &)
{
    context_.multiAnimations = true;
    StateBase::equipWeapon();
    RotateStateBase::setCurrentAnim();
}

void ArmedRotateState::update(const AimStopEvent &)
{
    if (drawArrowEndSub_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*drawArrowEndSub_);
        drawArrowEndSub_ = std::nullopt;
    }
}

void ArmedRotateState::onLeave()
{
    drawArrowEndSub_ = std::nullopt;
}
}  // namespace Components
}  // namespace GameEngine
