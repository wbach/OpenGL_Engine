#include "DrawArrowCrouchWalkAndRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowCrouchWalkAndRotateState::DrawArrowCrouchWalkAndRotateState(FsmContext& context)
    : DrawArrowStateBase(context, context.upperBodyGroupName)
    , MoveAndRotateStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.crouch,
                             context.animClipNames.armed.posture.crouched.rotate}
    , context_{context}
{
}

void DrawArrowCrouchWalkAndRotateState::onEnter(ArmedSprintAndRotateState&, const DrawArrowEvent&)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Sprint;
}

void DrawArrowCrouchWalkAndRotateState::onEnter(ArmedRunAndRotateState&, const DrawArrowEvent&)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Walk;
}

void DrawArrowCrouchWalkAndRotateState::onEnter(ArmedWalkAndRotateState&, const DrawArrowEvent&)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Run;
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const DrawArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const ReloadArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const MoveForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const MoveBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowCrouchWalkAndRotateState::onEnter(const WalkChangeStateEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowCrouchWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowCrouchWalkAndRotateState::update(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::update(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::update(const RotateTargetEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::update(const MoveForwardEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::update(const MoveBackwardEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::update(const MoveLeftEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::update(const MoveRightEvent& event)
{
    MoveStateBase::update(event);
}

void DrawArrowCrouchWalkAndRotateState::onMoveInactivity()
{
    RotateStateBase::setCurrentAnim();
}

void DrawArrowCrouchWalkAndRotateState::onLeave(const AimStopEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowCrouchWalkAndRotateState::onLeave(const WeaponStateEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowCrouchWalkAndRotateState::onLeave(const SprintStateChangeEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
