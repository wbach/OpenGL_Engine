#include "AimState.h"

#include "../FsmContext.h"
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

void AimState::onEnter(const EndRotationEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimState::onEnter(const EndForwardMoveEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimState::onEnter(const EndBackwardMoveEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimState::onEnter(const EndMoveLeftEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimState::onEnter(const EndMoveRightEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
