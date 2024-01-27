#include "ArmedRunState.h"

#include "../CharacterController.h"
#include "DisarmedRunState.h"

namespace GameEngine
{
namespace Components
{

ArmedRunState::ArmedRunState(FsmContext &context)
    : MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
{
}

void ArmedRunState::onEnter(const WeaponStateEvent &)
{
    context_.multiAnimations = true;
    StateBase::equipWeapon();
    MoveStateBase::setCurrentAnim();
}

void ArmedRunState::onEnter(DisarmedRunState &, const DrawArrowEvent &)
{
    drawArrowEndSub_ = context_.animator.SubscribeForAnimationFrame(
        context_.animClipNames.equip, [&]() { context_.characterController.fsm()->handle(DrawArrowEvent{}); });
}

void ArmedRunState::onEnter(const DrawArrowEvent &)
{
    context_.multiAnimations = true;
    StateBase::equipWeapon();
    MoveStateBase::setCurrentAnim();
}

void ArmedRunState::update(const AimStopEvent &)
{
    if (drawArrowEndSub_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*drawArrowEndSub_);
        drawArrowEndSub_ = std::nullopt;
    }
}

void ArmedRunState::onLeave()
{
    drawArrowEndSub_ = std::nullopt;
}

}  // namespace Components
}  // namespace GameEngine
