#include "AimCrouchWalkState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimCrouchWalkState::AimCrouchWalkState(FsmContext &context)
    : AimStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.walk}
    , context_{context}
{
}

void AimCrouchWalkState::onEnter(const AimStartEvent &event)
{
    AimStateBase::onEnter(event);
}

void AimCrouchWalkState::update(float dt)
{
    MoveStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimCrouchWalkState::onLeave(const AimStopEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimCrouchWalkState::onLeave(const SprintStateChangeEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimCrouchWalkState::onMoveInactivity()
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void AimCrouchWalkState::onLeave(const WeaponStateEvent &e)
{
    AimStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
