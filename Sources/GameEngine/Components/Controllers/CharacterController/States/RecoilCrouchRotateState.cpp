#include "RecoilCrouchRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilCrouchRotateState::RecoilCrouchRotateState(FsmContext& context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , RotateStateBase(context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.crouched.rotate, RotateStateBase::CameraRotationPolicy::none)
{
}

void RecoilCrouchRotateState::onEnter(const CrouchChangeStateEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilCrouchRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilCrouchRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilCrouchRotateState::onEnter(const RotateTargetEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilCrouchRotateState::onEnter(const EndForwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilCrouchRotateState::onEnter(const EndBackwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilCrouchRotateState::update(const RotateLeftEvent& event)
{
    onEnter(event);
}

void RecoilCrouchRotateState::update(const RotateRightEvent& event)
{
    onEnter(event);
}

void RecoilCrouchRotateState::update(const RotateTargetEvent& event)
{
    onEnter(event);
}

void RecoilCrouchRotateState::update(float dt)
{
    RotateStateBase::update(dt);
}

void RecoilCrouchRotateState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchRotateState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
