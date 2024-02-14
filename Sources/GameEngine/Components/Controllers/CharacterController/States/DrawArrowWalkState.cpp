#include "DrawArrowWalkState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowWalkState::DrawArrowWalkState(FsmContext &context)
    : DrawArrowStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , context_{context}
{
}

void DrawArrowWalkState::onEnter(ArmedSprintState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Sprint;
}

void DrawArrowWalkState::onEnter(ArmedWalkState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Walk;
}

void DrawArrowWalkState::onEnter(ArmedRunState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Run;
}

void DrawArrowWalkState::onEnter(const WalkChangeStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::update(const WalkForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onEnter(const WalkBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onEnter(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onEnter(const RunBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onEnter(const WalkForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onEnter(const DrawArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
    MoveStateBase::updateMoveState();
}

void DrawArrowWalkState::onEnter(const ReloadArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkState::update(float dt)
{
    MoveStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowWalkState::update(const WalkBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::update(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::update(const RunBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowWalkState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowWalkState::onLeave(const SprintStateChangeEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
