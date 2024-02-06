#include "IdleArmedChangeState.h"

#include <Logger/Log.h>

#include "../CharacterController.h"

namespace GameEngine
{
namespace Components
{
IdleArmedChangeState::IdleArmedChangeState(FsmContext &context)
    : ArmedChangeStateBase(context, std::nullopt)
    , IdleStateBase{context, context.animClipNames.disarmed.idle}
{
}
void IdleArmedChangeState::onEnter()
{
    drawArrowEventCalled_ = false;
}
void IdleArmedChangeState::onEnter(DisarmedIdleState &, const WeaponStateEvent &)
{
    DEBUG_LOG("onEnter(DisarmedIdleState&, const WeaponStateEvent &)");
    ArmedChangeStateBase::equipWeapon();
}
void IdleArmedChangeState::onEnter(ArmedIdleState &, const WeaponStateEvent &)
{
    DEBUG_LOG("onEnter(ArmedIdleState&, const WeaponStateEvent &)");
    ArmedChangeStateBase::disarmWeapon();
}
void IdleArmedChangeState::onEnter(DisarmedIdleState &, const DrawArrowEvent &)
{
    ArmedChangeStateBase::equipWeapon();
    drawArrowEventCalled_ = true;
}
void IdleArmedChangeState::update(const AimStopEvent &)
{
    drawArrowEventCalled_ = false;
}
void IdleArmedChangeState::onLeave(const EquipEndStateEvent &)
{
    DEBUG_LOG("onLeave " + std::to_string(drawArrowEventCalled_));
    if (drawArrowEventCalled_)
    {
        DEBUG_LOG("pushEventToQueue");
        ArmedChangeStateBase::context_.characterController.pushEventToQueue(DrawArrowEvent{});
    }
}
void IdleArmedChangeState::update(const DrawArrowEvent&)
{
    drawArrowEventCalled_ = true;
}
void IdleArmedChangeState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
