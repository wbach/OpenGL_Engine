#include "AimWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkAndRotateState::AimWalkAndRotateState(FsmContext &context)
    : AimStateBase(context)
    , MoveAndRotateStateBase{context, context.walkSpeed, context.animClipNames.armed.walk,
                             context.animClipNames.armed.rotateLeft, context.animClipNames.armed.rotateRight}
    , context_{context}
{
}
void AimWalkAndRotateState::onEnter()
{
    context_.multiAnimations = true;
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
}  // namespace Components
}  // namespace GameEngine
