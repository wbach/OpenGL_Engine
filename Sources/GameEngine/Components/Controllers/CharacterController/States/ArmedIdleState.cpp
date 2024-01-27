#include "ArmedIdleState.h"

#include "DisarmedIdleState.h"

namespace GameEngine
{
namespace Components
{
ArmedIdleState::ArmedIdleState(FsmContext &context)
    : IdleStateBase(context, context.animClipNames.armed.idle)
{
}

void ArmedIdleState::onEnter(const WeaponStateEvent &)
{
    StateBase::equipWeapon();
}

void ArmedIdleState::onEnter(DisarmedIdleState &idleState, const DrawArrowEvent &)
{
    drawArrowEndSub_ = idleState.drawArrowEndSub_;
}

void ArmedIdleState::onEnter(const DrawArrowEvent &)
{
    StateBase::equipWeapon();
}

void ArmedIdleState::update(const AimStopEvent &)
{
    if (drawArrowEndSub_)
    {
        context_.animator.UnSubscribeForAnimationFrame(*drawArrowEndSub_);
        drawArrowEndSub_ = std::nullopt;
    }
}

void ArmedIdleState::onLeave()
{
    drawArrowEndSub_ = std::nullopt;
}
}  // namespace Components
}  // namespace GameEngine
