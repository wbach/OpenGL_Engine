#include "RotateableRunState.h"
#include <Input/InputManager.h>

#include <Logger/Log.h>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
RotateableRunState::RotateableRunState(Context& context, const vec3& relativeCamerePosition)
    : context{context}
    , relativeCamerePosition(relativeCamerePosition, 1.f)
    , lookAtLocalPosition(relativeCamerePosition, 1.f)
{
    lookAtLocalPosition.z *= -1.f;
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
    auto worldCameraPosition = context.gameObject.GetWorldTransform().GetMatrix() * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = context.gameObject.GetWorldTransform().GetMatrix() * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void RotateableRunState::update(const InitEvent&)
{
    DEBUG_LOG("init tp camera");
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}

const vec4& RotateableRunState::getRelativeCamerePosition() const
{
    return relativeCamerePosition;
}

const vec4& RotateableRunState::getLookAtPosition() const
{
    return lookAtLocalPosition;
}

}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
