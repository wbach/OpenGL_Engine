#include "DrawArrowWalkAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowWalkAndRotateState::DrawArrowWalkAndRotateState(FsmContext &context)
    : DrawArrowStateBase(context)
    , MoveStateBase{context, context.walkSpeed, context.animClipNames.armed.walk.forward,
                    context.animClipNames.armed.walk.backward}
    , RotateStateBase(context, context.walkSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}

void DrawArrowWalkAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void DrawArrowWalkAndRotateState::onEnter(const WalkChangeStateEvent & event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const WalkBackwardEvent & event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const WalkForwardEvent & event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const DrawArrowEvent & event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowWalkAndRotateState::onEnter(const RotateRightEvent & event)
{
    RotateStateBase::update(event);
}

void DrawArrowWalkAndRotateState::onEnter(const RotateLeftEvent &event)
{
     RotateStateBase::update(event);
}
void DrawArrowWalkAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
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
