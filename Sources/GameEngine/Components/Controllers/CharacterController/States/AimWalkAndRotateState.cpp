#include "AimWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimWalkAndRotateState::AimWalkAndRotateState(FsmContext &context)
    : AimStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , RotateStateBase(context, context.walkSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}
void AimWalkAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void AimWalkAndRotateState::onEnter(const WalkChangeStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void AimWalkAndRotateState::onEnter(const WalkBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void AimWalkAndRotateState::onEnter(const WalkForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void AimWalkAndRotateState::onEnter(const AimStartEvent &event)
{
    AimStateBase::onEnter(event);
}

void AimWalkAndRotateState::onEnter(const RotateRightEvent &event)
{
    RotateStateBase::update(event);
}

void AimWalkAndRotateState::onEnter(const RotateLeftEvent &event)
{
    RotateStateBase::update(event);
}
void AimWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
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
