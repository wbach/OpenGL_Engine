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
    , IdleStateBase(context, context.animClipNames.armed.crouchIdle, context.lowerBodyGroupName)
    , context_{context}
{
}
void RecoilCrouchState::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilCrouchState::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void RecoilCrouchState::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
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
