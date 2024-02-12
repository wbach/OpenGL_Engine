#include "FollowingState.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <Display/DisplayManager.hpp>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
FollowingState::FollowingState(Context& context, const vec3& rcp)
    : context{context}
    , thridPersonCameraComponent{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , referenceRelativeCamerePosition{rcp, 1.f}
    , lookAtLocalPosition(rcp, 1.f)
{
    lookAtLocalPosition.z *= -1.f;
}
FollowingState::~FollowingState()
{
}
void FollowingState::onEnter()
{
    update(InitEvent{});
}
void FollowingState::cameraUpdate()
{
    if (triggerEventIfMouseMoveDetect())
    {
        return;
    }

    autoFallowCamera();
}
void FollowingState::update(const InitEvent&)
{
    DEBUG_LOG("init tp camera");
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}
const vec4& FollowingState::getRelativeCamerePosition() const
{
    return referenceRelativeCamerePosition;
}

const vec4& FollowingState::getLookAtPosition() const
{
    return lookAtLocalPosition;
}
bool FollowingState::triggerEventIfMouseMoveDetect()
{
    auto v = context.inputManager.CalcualteMouseMove();
    if (v.x != 0 or v.y != 0)
    {
        if (thridPersonCameraComponent)
            thridPersonCameraComponent->pushEventToQueue(MouseMoveEvent{});
        return true;
    }
    return false;
}

void FollowingState::autoFallowCamera()
{
    auto worldCameraPosition = context.gameObject.GetWorldTransform().GetMatrix() * referenceRelativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = context.gameObject.GetWorldTransform().GetMatrix() * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
