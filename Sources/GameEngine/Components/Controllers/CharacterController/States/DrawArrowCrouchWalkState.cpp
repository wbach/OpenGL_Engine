#include "DrawArrowCrouchWalkState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowCrouchWalkState::DrawArrowCrouchWalkState(FsmContext &context)
    : DrawArrowStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.walkSpeed, context.animClipNames.armed.movement.crouch}
    , context_{context}
{
}

void DrawArrowCrouchWalkState::onEnter(ArmedSprintState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Sprint;
}

void DrawArrowCrouchWalkState::onEnter(ArmedWalkState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Walk;
}

void DrawArrowCrouchWalkState::onEnter(ArmedRunState &, const DrawArrowEvent &)
{
    context_.aimEnteringState = FsmContext::AimEnteringState::Run;
}

void DrawArrowCrouchWalkState::onEnter(const DrawArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowCrouchWalkState::onEnter(const ReloadArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
    MoveStateBase::setCurrentAnimIfNeeded();
}

void DrawArrowCrouchWalkState::update(float dt)
{
    MoveStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowCrouchWalkState::onMoveInactivity()
{
    MoveStateBase::setAnim(context_.animClipNames.armed.posture.crouched.idle);
}

void DrawArrowCrouchWalkState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowCrouchWalkState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowCrouchWalkState::onLeave(const SprintStateChangeEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
