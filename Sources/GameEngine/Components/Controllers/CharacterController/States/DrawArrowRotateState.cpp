#include "DrawArrowRotateState.h"

#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRotateState::DrawArrowRotateState(FsmContext& context)
    : DrawArrowStateBase{context}
    , RotateStateBase{context, context.runSpeed.leftRight, context.animClipNames.armed.rotateLeft,
                      context.animClipNames.armed.rotateRight}
    , context_{context}
{
}
void DrawArrowRotateState::onEnter(const DrawArrowEvent& event)
{
    context_.multiAnimations = true;
    DrawArrowStateBase::onEnter(event);
}

void DrawArrowRotateState::onEnter(const ReloadArrowEvent& event)
{
    context_.multiAnimations = true;
    DrawArrowStateBase::onEnter(event);
}
void DrawArrowRotateState::onEnter(const RotateLeftEvent& event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}
void DrawArrowRotateState::onEnter(const RotateRightEvent& event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}

void DrawArrowRotateState::onEnter(const EndForwardMoveEvent& event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}

void DrawArrowRotateState::onEnter(const EndBackwardMoveEvent& event)
{
    context_.multiAnimations = true;
    RotateStateBase::onEnter(event);
}
void DrawArrowRotateState::update(const RotateLeftEvent& event)
{
    onEnter(event);
}
void DrawArrowRotateState::update(const RotateRightEvent& event)
{
    onEnter(event);
}
void DrawArrowRotateState::update(float dt)
{
    RotateStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}

void DrawArrowRotateState::onLeave(const AimStopEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRotateState::onLeave(const WeaponStateEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowRotateState::onLeave(const SprintStartEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
