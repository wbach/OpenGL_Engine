#include "RecoilCrouchWalkAndRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"


namespace GameEngine
{
namespace Components
{
RecoilCrouchWalkAndRotateState::RecoilCrouchWalkAndRotateState(FsmContext &context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.crouch,
                             context.animClipNames.armed.posture.crouched.rotate}
{
}

void RecoilCrouchWalkAndRotateState::onEnter(const AttackEvent &event)
{
    RecoilStateBase::onEnter(event);
}

void RecoilCrouchWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    RecoilStateBase::update(dt);
}

void RecoilCrouchWalkAndRotateState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchWalkAndRotateState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchWalkAndRotateState::onLeave(const SprintStateChangeEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}
}  // namespace Components
}  // namespace GameEngine
