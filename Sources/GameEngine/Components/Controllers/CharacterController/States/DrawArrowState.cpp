#include "DrawArrowState.h"

#include "../FsmContext.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowState::DrawArrowState(FsmContext &context)
    : DrawArrowStateBase{context, std::nullopt}
{
}
void DrawArrowState::onEnter(const EndRotationEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void DrawArrowState::onEnter(const EndForwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void DrawArrowState::onEnter(const EndBackwardMoveEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void DrawArrowState::onEnter(const CrouchChangeStateEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void DrawArrowState::onEnter(const EndMoveLeftEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}

void DrawArrowState::onEnter(const EndMoveRightEvent &)
{
    context_.animator.StopAnimation(context_.lowerBodyGroupName);
}
void DrawArrowState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
