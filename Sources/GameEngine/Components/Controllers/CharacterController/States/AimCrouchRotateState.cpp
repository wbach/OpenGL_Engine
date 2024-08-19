#include "AimCrouchRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimCrouchRotateState::AimCrouchRotateState(FsmContext &context)
    : AimStateBase(context, context.upperBodyGroupName)
    , RotateStateBase(context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.crouched.rotate, RotateStateBase::CameraRotationPolicy::none)
    , context_{context}
{
}

void AimCrouchRotateState::onEnter(const AimStartEvent &event)
{
    // /*DISABLED*/ DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    AimStateBase::onEnter(event);
    RotateStateBase::update(event);
}

void AimCrouchRotateState::onEnter(const CrouchChangeStateEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimCrouchRotateState::onEnter(const RotateLeftEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimCrouchRotateState::onEnter(const RotateRightEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimCrouchRotateState::onEnter(const RotateTargetEvent &event)
{
    // /*DISABLED*/ DEBUG_LOG("AimCrouchRotateState::onEnter(const RotateTargetEvent& event)");
    RotateStateBase::onEnter(event);
}

void AimCrouchRotateState::onEnter(const EndForwardMoveEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimCrouchRotateState::onEnter(const EndBackwardMoveEvent &event)
{
    RotateStateBase::onEnter(event);
}

void AimCrouchRotateState::update(const RotateLeftEvent &event)
{
    onEnter(event);
}
void AimCrouchRotateState::update(const RotateRightEvent &event)
{
    onEnter(event);
}
void AimCrouchRotateState::update(const RotateTargetEvent &event)
{
    onEnter(event);
}
void AimCrouchRotateState::update(float dt)
{
    RotateStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimCrouchRotateState::onLeave(const AimStopEvent &event)
{
    // /*DISABLED*/ DEBUG_LOG("onLeave(AimStopEvent)");
    AimStateBase::onLeave(event);
}

void AimCrouchRotateState::onLeave(const WeaponStateEvent &event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
