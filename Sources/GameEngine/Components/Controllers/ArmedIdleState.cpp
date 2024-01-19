#include "ArmedIdleState.h"

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
}  // namespace Components
}  // namespace GameEngine
