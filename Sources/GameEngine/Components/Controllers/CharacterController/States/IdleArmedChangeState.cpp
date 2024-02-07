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
    , context_{context}
{
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
void IdleArmedChangeState::onEnter(DisarmedIdleState &, const DrawArrowEvent &e)
{
    ArmedChangeStateBase::equipWeapon();
    ArmedChangeStateBase::update(e);
}
void IdleArmedChangeState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
