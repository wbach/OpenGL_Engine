#include "DrawArrowRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowRotateState::DrawArrowRotateState(FsmContext& context)
    : DrawArrowStateBase{context, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.stand.rotate, RotateStateBase::CameraRotationPolicy::none}
{
}
void DrawArrowRotateState::onEnter(const EndForwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowRotateState::onEnter(const EndBackwardMoveEvent& event)
{
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
void DrawArrowRotateState::update(const RotateTargetEvent& event)
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
}  // namespace Components
}  // namespace GameEngine
