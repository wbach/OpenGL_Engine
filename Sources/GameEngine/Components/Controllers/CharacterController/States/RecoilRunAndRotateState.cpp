#include "RecoilRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilRunAndRotateState::RecoilRunAndRotateState(FsmContext& context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, std::nullopt, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
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

void RecoilRunAndRotateState::onLeave(const SprintStateChangeEvent& e)
{
    RecoilStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
