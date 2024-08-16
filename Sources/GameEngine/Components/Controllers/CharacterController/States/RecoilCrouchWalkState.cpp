#include "RecoilCrouchWalkState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilCrouchWalkState::RecoilCrouchWalkState(FsmContext& context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.crouch}
    , context_{context}
{
}

void RecoilCrouchWalkState::onEnter(const AttackEvent& event)
{
    RecoilStateBase::onEnter(event);
}
void RecoilCrouchWalkState::update(float dt)
{
    MoveStateBase::update(dt);
    RecoilStateBase::update(dt);
}

void RecoilCrouchWalkState::onMoveInactivity()
{
    MoveStateBase::setAnim(context_.animClipNames.armed.posture.crouched.idle);
}
void RecoilCrouchWalkState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchWalkState::onLeave(const SprintStateChangeEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilCrouchWalkState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
