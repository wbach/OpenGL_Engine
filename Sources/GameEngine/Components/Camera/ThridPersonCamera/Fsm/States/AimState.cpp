#include "AimState.h"
#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Objects/GameObject.h"
#include "Input/InputManager.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
AimState::AimState(Context& context)
    : context{context}
    , relativeCamerePosition(-0.25f, 1.0f, -0.75f, 1.f)

{
}

void AimState::onEnter(const StartAimEvent&)
{
    DEBUG_LOG("StartAimEvent");
    context.inputManager.ShowCursor(true);
    context.inputManager.SetReleativeMouseMode(false);
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
    cameraUpdate();
}

void AimState::cameraUpdate()
{
    DEBUG_LOG("AimState::cameraUpdate()");
    auto worldCameraPosition = context.gameObject.GetWorldTransform().GetMatrix() * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtLocalPosition = relativeCamerePosition;
    lookAtLocalPosition.z *= -1.f;
    auto lookAtPosition = context.gameObject.GetWorldTransform().GetMatrix() * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
