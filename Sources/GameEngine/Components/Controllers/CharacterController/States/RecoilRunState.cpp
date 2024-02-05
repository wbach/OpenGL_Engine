#include "RecoilRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
RecoilRunState::RecoilRunState(FsmContext &context)
    : RecoilStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , context_{context}
{
}

void RecoilRunState::onEnter(const AttackEvent &event)
{
    RecoilStateBase::onEnter(event);
}

void RecoilRunState::onEnter(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunState::onEnter(const RunBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunState::onEnter(const EndRotationEvent &event)
{
}

void RecoilRunState::onEnter(const WalkChangeStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunState::update(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunState::update(const RunBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void RecoilRunState::onLeave(const AimStopEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilRunState::onLeave(const WeaponStateEvent &e)
{
    RecoilStateBase::onLeave(e);
}

void RecoilRunState::onLeave(const SprintStateChangeEvent &e)
{
    RecoilStateBase::onLeave(e);
}
void RecoilRunState::update(float dt)
{
    MoveStateBase::update(dt);
    RecoilStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
