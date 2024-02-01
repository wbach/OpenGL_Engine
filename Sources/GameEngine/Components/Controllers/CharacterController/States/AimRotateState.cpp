#include "AimRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
AimRotateState::AimRotateState(FsmContext &context)
    : AimStateBase(context)
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}

void AimRotateState::onEnter(const AimStartEvent &event)
{
    context_.multiAnimations = true;
    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
    AimStateBase::onEnter(event);
    RotateStateBase::update(event);
}

void AimRotateState::onEnter(const RotateLeftEvent &event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const RotateRightEvent &event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const EndForwardMoveEvent & event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}

void AimRotateState::onEnter(const EndBackwardMoveEvent & event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}

void AimRotateState::update(const RotateLeftEvent &event)
{
    onEnter(event);
}

void AimRotateState::update(const RotateRightEvent &event)
{
    onEnter(event);
}

void AimRotateState::update(float dt)
{
    RotateStateBase::update(dt);
    AimStateBase::update(dt);
}

void AimRotateState::onLeave(const AimStopEvent &event)
{
    DEBUG_LOG("onLeave(AimStopEvent)");
    AimStateBase::onLeave(event);
}

void AimRotateState::onLeave(const WeaponStateEvent &event)
{
    AimStateBase::onLeave(event);
}
}  // namespace Components
}  // namespace GameEngine
