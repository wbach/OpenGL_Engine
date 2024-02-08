#include "DrawArrowRunState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRunState::DrawArrowRunState(FsmContext &context)
    : DrawArrowStateBase(context, context.upperBodyGroupName)
    , MoveStateBase{context, context.lowerBodyGroupName, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , context_{context}
{
}

void DrawArrowRunState::onEnter(const DrawArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
    MoveStateBase::updateMoveState();
}

void DrawArrowRunState::onEnter(const ReloadArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowRunState::onEnter(const RunForwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowRunState::onEnter(const RunBackwardEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowRunState::onEnter(const WalkChangeStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowRunState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunState::onLeave(const SprintStateChangeEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunState::update(float dt)
{
    MoveStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowRunState::update(const RunForwardEvent &event)
{
    MoveStateBase::update(event);
}

void DrawArrowRunState::update(const RunBackwardEvent &event)
{
    MoveStateBase::update(event);
}
}  // namespace Components
}  // namespace GameEngine
