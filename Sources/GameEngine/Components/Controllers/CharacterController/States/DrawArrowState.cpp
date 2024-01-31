#include "DrawArrowState.h"

#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
DrawArrowState::DrawArrowState(FsmContext &context)
    : DrawArrowStateBase{context}
{
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
