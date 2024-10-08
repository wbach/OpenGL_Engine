#include "AimWalkAndRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkAndRotateState::AimWalkAndRotateState(FsmContext &context)
    : AimStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.walk,
                             context.animClipNames.armed.posture.stand.rotate}
{
}

void AimWalkAndRotateState::onEnter(const AimStartEvent &event)
{
    AimStateBase::onEnter(event);
}

void AimWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimWalkAndRotateState::onLeave(const AimStopEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimWalkAndRotateState::onLeave(const WeaponStateEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimWalkAndRotateState::onLeave(const SprintStateChangeEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}
}  // namespace Components
}  // namespace GameEngine
