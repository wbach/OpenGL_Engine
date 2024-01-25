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
    DEBUG_LOG("onEnter AimStartEvent clip : " + context_.animClipNames.aimIdle);
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
void DrawArrowRotateState::update(const RotateLeftEvent& event)
{
    onEnter(event);
}
void DrawArrowRotateState::update(const RotateRightEvent& event)
{
    onEnter(event);
}
void DrawArrowRotateState::update(float)
{
}
}  // namespace Components
}  // namespace GameEngine
