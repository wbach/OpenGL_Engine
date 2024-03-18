#include "DrawArrowWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowWalkAndRotateState::DrawArrowWalkAndRotateState(FsmContext& context)
    : DrawArrowStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context,
                             context.lowerBodyGroupName,
                             context.walkSpeed,
                             context.animClipNames.armed.walk,
                             context.animClipNames.armed.rotateLeft,
                             context.animClipNames.armed.rotateRight}
    , context_{context}
{
}

void DrawArrowWalkAndRotateState::onEnter(ArmedSprintAndRotateState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Sprint;
}

void DrawArrowWalkAndRotateState::onEnter(ArmedRunAndRotateState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Walk;
}

void DrawArrowWalkAndRotateState::onEnter(ArmedWalkAndRotateState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Run;
}

void DrawArrowWalkAndRotateState::onEnter(const DrawArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::onEnter(const ReloadArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::onEnter(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::onEnter(const MoveForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const MoveBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const WalkChangeStateEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowWalkAndRotateState::update(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::update(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::update(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::update(const MoveForwardEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowWalkAndRotateState::update(const MoveBackwardEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowWalkAndRotateState::update(const MoveLeftEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowWalkAndRotateState::update(const MoveRightEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}

void DrawArrowWalkAndRotateState::onLeave(const AimStopEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowWalkAndRotateState::onLeave(const WeaponStateEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowWalkAndRotateState::onLeave(const SprintStateChangeEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
