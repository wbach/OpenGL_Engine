#include "AimCrouchState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimCrouchState::AimCrouchState(FsmContext& context)
    : AimStateBase(context, context.upperBodyGroupName)
    , IdleStateBase(context, context.animClipNames.armed.posture.crouched.idle, context.lowerBodyGroupName)
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
    IdleStateBase::setIdleAnim();
}

void AimCrouchState::onEnter(const EndForwardMoveEvent&)
{
    IdleStateBase::setIdleAnim();
}

void AimCrouchState::onEnter(const EndBackwardMoveEvent&)
{
    IdleStateBase::setIdleAnim();
}

void AimCrouchState::onEnter(const EndMoveLeftEvent&)
{
    IdleStateBase::setIdleAnim();
}

void AimCrouchState::onEnter(const EndMoveRightEvent&)
{
    IdleStateBase::setIdleAnim();
}

void AimCrouchState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
