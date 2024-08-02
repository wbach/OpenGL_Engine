#include "AimCrouchState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimCrouchState::AimCrouchState(FsmContext& context)
    : AimStateBase(context, context.upperBodyGroupName)
    , IdleStateBase(context, context.animClipNames.armed.crouchIdle, context.lowerBodyGroupName)
    , context_{context}
{
}

void AimCrouchState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimCrouchState::update(float dt)
{
    AimStateBase::update(dt);
}

void AimCrouchState::onEnter(const EndRotationEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimCrouchState::onEnter(const EndForwardMoveEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimCrouchState::onEnter(const EndBackwardMoveEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimCrouchState::onEnter(const EndMoveLeftEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimCrouchState::onEnter(const EndMoveRightEvent&)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimCrouchState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
