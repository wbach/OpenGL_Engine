#include "ScriptedState.h"

#include "GameEngine/Components/Camera/CameraComponent.h"

namespace GameEngine
{
namespace Components
{
Camera::ScriptedState::ScriptedState(Context& context)
    : context(context)
{
}
void Camera::ScriptedState::onEnter()
{
    context.camera.setOnUpdate(nullptr);
}
}  // namespace Components
}  // namespace GameEngine
