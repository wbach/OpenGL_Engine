#include "RecoilRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilRunAndRotateState::RecoilRunAndRotateState(FsmContext& context)
    : RecoilStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}
void RecoilRunAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void RecoilRunAndRotateState::onEnter(const AttackEvent& event)
{
    RecoilStateBase::onEnter(event);
}

void RecoilRunAndRotateState::onEnter(const RunForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunAndRotateState::onEnter(const RunBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunAndRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void RecoilRunAndRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilRunAndRotateState::onEnter(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}

void RecoilRunAndRotateState::onEnter(const WalkChangeStateEvent& event)
{
    MoveStateBase::onEnter(event);
}
void RecoilRunAndRotateState::update(const RunForwardEvent& event)
{
    MoveStateBase::update(event);
}
void RecoilRunAndRotateState::update(const RunBackwardEvent& event)
{
    MoveStateBase::update(event);
}
void RecoilRunAndRotateState::update(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilRunAndRotateState::update(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilRunAndRotateState::update(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilRunAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}

void RecoilRunAndRotateState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilRunAndRotateState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
