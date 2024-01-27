#include "RecoilWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilWalkAndRotateState::RecoilWalkAndRotateState(FsmContext& context)
    : RecoilStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , RotateStateBase(context, context.walkSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}
void RecoilWalkAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void RecoilWalkAndRotateState::onEnter(const WalkChangeStateEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const WalkBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const WalkForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const AttackEvent& event)
{
    RecoilStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void RecoilWalkAndRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void RecoilWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}
void RecoilWalkAndRotateState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkAndRotateState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
