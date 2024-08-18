#include "AimCrouchWalkAndRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimCrouchWalkAndRotateState::AimCrouchWalkAndRotateState(FsmContext &context)
    : AimStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.crouch,
                             context.animClipNames.armed.posture.crouched.rotate}
{
}

void AimCrouchWalkAndRotateState::onEnter(const AimStartEvent &event)
{
    AimStateBase::onEnter(event);
}

void AimCrouchWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimCrouchWalkAndRotateState::onLeave(const AimStopEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimCrouchWalkAndRotateState::onLeave(const WeaponStateEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimCrouchWalkAndRotateState::onLeave(const SprintStateChangeEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimCrouchWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}
}  // namespace Components
}  // namespace GameEngine
