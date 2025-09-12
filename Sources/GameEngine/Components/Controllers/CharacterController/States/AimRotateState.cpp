#include "AimRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRotateState::AimRotateState(FsmContext &context)
    : AimStateBase(context, context.upperBodyGroupName)
    , RotateStateBase(context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.stand.rotate, RotateStateBase::CameraRotationPolicy::none)
    , context_{context}
{
}

void AimRotateState::onEnter(const AimStartEvent &event)
{
    AimStateBase::onEnter(event);
    RotateStateBase::update(event);
}

void AimRotateState::onEnter(const CrouchChangeStateEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const RotateLeftEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const RotateRightEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const RotateTargetEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const EndForwardMoveEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const EndBackwardMoveEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimRotateState::update(const RotateLeftEvent &event)
{
    onEnter(event);
}
void AimRotateState::update(const RotateRightEvent &event)
{
    onEnter(event);
}
void AimRotateState::update(const RotateTargetEvent &event)
{
    onEnter(event);
}
void AimRotateState::update(float dt)
{
    RotateStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimRotateState::onLeave(const AimStopEvent &event)
{
    AimStateBase::onLeave(event);
}

void AimRotateState::onLeave(const WeaponStateEvent &event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
