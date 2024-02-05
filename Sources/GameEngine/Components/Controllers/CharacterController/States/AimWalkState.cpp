#include "AimWalkState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkState::AimWalkState(FsmContext &context)
    : AimStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , context_{context}
{
}

void AimWalkState::onEnter(const AimStartEvent &event)
{
    AimStateBase::onEnter(event);
}

void AimWalkState::update(float dt)
{
    MoveStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimWalkState::onLeave(const AimStopEvent &e)
{
    AimStateBase::onLeave(e);
}

void AimWalkState::onLeave(const SprintStateChangeEvent & e)
{
    AimStateBase::onLeave(e);
}

void AimWalkState::onLeave(const WeaponStateEvent &e)
{
    AimStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
