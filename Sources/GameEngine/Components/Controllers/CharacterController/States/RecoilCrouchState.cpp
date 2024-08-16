#include "RecoilCrouchState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilCrouchState::RecoilCrouchState(FsmContext &context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , IdleStateBase(context, context.animClipNames.armed.posture.crouched.idle, context.lowerBodyGroupName)
    , context_{context}
{
}
void RecoilCrouchState::onEnter(const EndRotationEvent &)
{
    IdleStateBase::setIdleAnim();
}
void RecoilCrouchState::onEnter(const EndForwardMoveEvent &)
{
    IdleStateBase::setIdleAnim();
}
void RecoilCrouchState::onEnter(const EndBackwardMoveEvent &)
{
    IdleStateBase::setIdleAnim();
}

void RecoilCrouchState::onEnter(const EndMoveLeftEvent &)
{
    IdleStateBase::setIdleAnim();
}

void RecoilCrouchState::onEnter(const EndMoveRightEvent &)
{
    IdleStateBase::setIdleAnim();
}
void RecoilCrouchState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchState::update(float dt)
{
    RecoilStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
