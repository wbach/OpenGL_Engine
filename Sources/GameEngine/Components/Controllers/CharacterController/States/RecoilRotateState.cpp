#include "RecoilRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilRotateState::RecoilRotateState(FsmContext& context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , RotateStateBase(context, context.lowerBodyGroupName, context.runSpeed.leftRight,
                      context.animClipNames.armed.rotateLeft, context.animClipNames.armed.rotateRight)
    , context_{context}
{
}

void RecoilRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilRotateState::onEnter(const RotateTargetEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilRotateState::onEnter(const EndForwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilRotateState::onEnter(const EndBackwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}

void RecoilRotateState::update(const RotateLeftEvent& event)
{
    onEnter(event);
}

void RecoilRotateState::update(const RotateRightEvent& event)
{
    onEnter(event);
}

void RecoilRotateState::update(const RotateTargetEvent& event)
{
    onEnter(event);
}

void RecoilRotateState::update(float dt)
{
    RotateStateBase::update(dt);
}

void RecoilRotateState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilRotateState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilRotateState::onLeave(const SprintStartEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
