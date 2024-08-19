#include "DrawArrowCrouchRotateState.h"

#include "../FsmContext.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowCrouchRotateState::DrawArrowCrouchRotateState(FsmContext& context)
    : DrawArrowStateBase{context, context.upperBodyGroupName}
    , RotateStateBase{context, context.lowerBodyGroupName, context.runSpeed.rotate,
                      context.animClipNames.armed.posture.crouched.rotate, RotateStateBase::CameraRotationPolicy::none}
{
}
void DrawArrowCrouchRotateState::onEnter(const DrawArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const ReloadArrowEvent& event)
{
    DrawArrowStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const RotateLeftEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const RotateRightEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const RotateTargetEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const EndForwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const EndBackwardMoveEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::onEnter(const CrouchChangeStateEvent& event)
{
    RotateStateBase::onEnter(event);
}
void DrawArrowCrouchRotateState::update(const RotateLeftEvent& event)
{
    onEnter(event);
}
void DrawArrowCrouchRotateState::update(const RotateRightEvent& event)
{
    onEnter(event);
}
void DrawArrowCrouchRotateState::update(const RotateTargetEvent& event)
{
    onEnter(event);
}
void DrawArrowCrouchRotateState::update(float dt)
{
    RotateStateBase::update(dt);
    DrawArrowStateBase::update(dt);
}
void DrawArrowCrouchRotateState::onLeave(const AimStopEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
void DrawArrowCrouchRotateState::onLeave(const WeaponStateEvent& e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
