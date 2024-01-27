#include "DrawArrowRunAndRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRunAndRotateState::DrawArrowRunAndRotateState(FsmContext& context)
    : DrawArrowStateBase(context)
    , MoveStateBase{context, context.runSpeed, context.animClipNames.armed.run.forward,
                    context.animClipNames.armed.run.backward}
    , RotateStateBase(context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight)
    , context_{context}
{
}
void DrawArrowRunAndRotateState::onEnter()
{
    context_.multiAnimations = true;
}

void DrawArrowRunAndRotateState::onEnter(const DrawArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowRunAndRotateState::onEnter(const RunForwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowRunAndRotateState::onEnter(const RunBackwardEvent& event)
{
    MoveStateBase::onEnter(event);
}

void DrawArrowRunAndRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowRunAndRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::update(event);
}

void DrawArrowRunAndRotateState::update(float dt)
{
    MoveStateBase::update(dt);
    RotateStateBase::update(dt);
}

void DrawArrowRunAndRotateState::onLeave(const AimStopEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRunAndRotateState::onLeave(const WeaponStateEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
