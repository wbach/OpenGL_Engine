#include "RecoilWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilWalkAndRotateState::RecoilWalkAndRotateState(FsmContext& context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.walk}
    , RotateStateBase(context, context.lowerBodyGroupName, context.walkSpeed.rotate, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
{
}

void RecoilWalkAndRotateState::onEnter(const WalkChangeStateEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const MoveBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const MoveForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const MoveLeftEvent& event)
{
    MoveStateBase::onEnter(event);
}

void RecoilWalkAndRotateState::onEnter(const MoveRightEvent& event)
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

void RecoilWalkAndRotateState::onEnter(const RotateTargetEvent& event)
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
void RecoilWalkAndRotateState::update(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilWalkAndRotateState::update(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilWalkAndRotateState::update(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}
void RecoilWalkAndRotateState::update(const MoveForwardEvent& event)
{
    MoveStateBase::update(event);
}
void RecoilWalkAndRotateState::update(const MoveBackwardEvent& event)
{
    MoveStateBase::update(event);
}

void RecoilWalkAndRotateState::update(const MoveLeftEvent& event)
{
    MoveStateBase::update(event);
}

void RecoilWalkAndRotateState::update(const MoveRightEvent& event)
{
    MoveStateBase::update(event);
}

void RecoilWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}

void RecoilWalkAndRotateState::onLeave(const AimStopEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkAndRotateState::onLeave(const WeaponStateEvent& e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilWalkAndRotateState::onLeave(const SprintStateChangeEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
