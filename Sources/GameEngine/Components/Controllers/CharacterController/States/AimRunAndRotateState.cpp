#include "AimRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRunAndRotateState::AimRunAndRotateState(FsmContext& context)
    : AimStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}
void AimRunAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void AimRunAndRotateState::onEnter(const AimStartEvent& event)
{
    AimStateBase::onEnter(event);
}

void AimRunAndRotateState::onEnter(const RunForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void AimRunAndRotateState::onEnter(const RunBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void AimRunAndRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void AimRunAndRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void AimRunAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}

void AimRunAndRotateState::onLeave(const AimStopEvent& event)
{
    AimStateBase::onLeave(event);
}

void AimRunAndRotateState::onLeave(const WeaponStateEvent& event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
