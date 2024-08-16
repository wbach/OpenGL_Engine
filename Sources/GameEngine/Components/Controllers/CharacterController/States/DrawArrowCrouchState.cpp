#include "DrawArrowCrouchState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowCrouchState::DrawArrowCrouchState(FsmContext &context)
    : DrawArrowStateBase{context, context.upperBodyGroupName}
    , IdleStateBase(context, context.animClipNames.armed.posture.crouched.idle, context.lowerBodyGroupName)
    , context_{context}
{
}
void DrawArrowCrouchState::onEnter()
{
    IdleStateBase::setIdleAnim();
}
void DrawArrowCrouchState::onEnter(const EndRotationEvent &)
{
    IdleStateBase::setIdleAnim();
}
void DrawArrowCrouchState::onEnter(const EndForwardMoveEvent &)
{
    IdleStateBase::setIdleAnim();
}
void DrawArrowCrouchState::onEnter(const EndBackwardMoveEvent &)
{
    IdleStateBase::setIdleAnim();
}

void DrawArrowCrouchState::onEnter(const EndMoveLeftEvent &)
{
    IdleStateBase::setIdleAnim();
}

void DrawArrowCrouchState::onEnter(const EndMoveRightEvent &)
{
    IdleStateBase::setIdleAnim();
}
void DrawArrowCrouchState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
void DrawArrowCrouchState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
void DrawArrowCrouchState::update(float dt)
{
    DrawArrowStateBase::update(dt);
}
}  // namespace Components
}  // namespace GameEngine
