#include "RotateableRunState.h"

#include <Logger/Log.h>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
RotateableRunState::RotateableRunState(Context& context)
    : keysSubscriptionsManager{context.inputManager}
    , context{context}
    , zoomSpeed_(0.1f)
    , relativeCamerePosition(-0.5f, 1.0f, -1.5f, 1.f)
{
}

RotateableRunState::~RotateableRunState()
{
}

void RotateableRunState::onEnter(const StopAimEvent&)
{
    update(InitEvent{});
}

void RotateableRunState::cameraUpdate()
{
    // DEBUG_LOG("RotateableRunState::cameraUpdate()");
    //  CalculateInput();

    auto worldCameraPosition = context.gameObject.GetWorldTransform().GetMatrix() * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtLocalPosition = relativeCamerePosition;
    lookAtLocalPosition.z *= -1.f;
    auto lookAtPosition = context.gameObject.GetWorldTransform().GetMatrix() * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void RotateableRunState::update(const InitEvent&)
{
    context.inputManager.ShowCursor(true);
    context.inputManager.SetReleativeMouseMode(false);
    DEBUG_LOG("init tp camera");
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
