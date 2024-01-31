#include "DrawArrowWalkState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowWalkState::DrawArrowWalkState(FsmContext &context)
    : DrawArrowStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , context_{context}
{
}
void DrawArrowWalkState::onEnter()
{
    context_.multiAnimations = true;
}

void DrawArrowWalkState::onEnter(const WalkChangeStateEvent &event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkState::onEnter(const WalkBackwardEvent &event)
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
}

void DrawArrowWalkState::onEnter(const ReloadArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkState::update(float dt)
{
    MoveStateBase::update(dt);
}

void DrawArrowWalkState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowWalkState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
