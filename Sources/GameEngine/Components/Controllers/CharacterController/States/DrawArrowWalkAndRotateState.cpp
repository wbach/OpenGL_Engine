#include "DrawArrowWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowWalkAndRotateState::DrawArrowWalkAndRotateState(FsmContext &context)
    : DrawArrowStateBase(context)
    , MoveAndRotateStateBase{context, context.walkSpeed, context.animClipNames.armed.walk,
                             context.animClipNames.armed.rotateLeft, context.animClipNames.armed.rotateRight}
    , context_{context}
{
}

void DrawArrowWalkAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void DrawArrowWalkAndRotateState::onEnter(const DrawArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const ReloadArrowEvent &event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowWalkAndRotateState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowWalkAndRotateState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
