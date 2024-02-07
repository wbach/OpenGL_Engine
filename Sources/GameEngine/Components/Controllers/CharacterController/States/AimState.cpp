#include "AimState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimState::AimState(FsmContext& context)
    : AimStateBase(context, std::nullopt)
{
}

void AimState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimState::onLeave(const SprintStartEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimState::onEnter(const EndRotationEvent&)
{
    // do nothing
}

void AimState::onEnter(const EndForwardMoveEvent&)
{
    // do nothing
}

void AimState::onEnter(const EndBackwardMoveEvent&)
{
    // do nothing
}

void AimState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
