#include "DrawArrowState.h"

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
void DrawArrowState::onEnter(const EndRotationEvent&)
{
    // do nothing
}
void DrawArrowState::onEnter(const EndForwardMoveEvent&)
{
    // do nothing
}
void DrawArrowState::onEnter(const EndBackwardMoveEvent&)
{
    // do nothing
}
void DrawArrowState::onLeave(const AimStopEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowState::onLeave(const WeaponStateEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}

void DrawArrowState::onLeave(const SprintStartEvent &e)
{
    DrawArrowStateBase::onLeave(e);
}
}  // namespace Components
}  // namespace GameEngine
