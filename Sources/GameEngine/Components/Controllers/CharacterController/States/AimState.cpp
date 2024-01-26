#include "AimState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimState::AimState(FsmContext& context)
    : AimStateBase(context)
{
}

void AimState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
