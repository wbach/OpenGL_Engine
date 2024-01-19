#include "DisarmedIdleState.h"

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

}  // namespace Components
}  // namespace GameEngine
