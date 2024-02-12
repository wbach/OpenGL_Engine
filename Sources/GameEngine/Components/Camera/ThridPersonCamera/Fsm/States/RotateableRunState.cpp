#include "RotateableRunState.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>

#include <Display/DisplayManager.hpp>

#include "GameEngine/Camera/CustomCamera.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace Camera
{
RotateableRunState::RotateableRunState(Context& context, const vec3& rcp)
    : context{context}
    , thridPersonCameraComponent{context.gameObject.GetComponent<ThridPersonCameraComponent>()}
    , referenceRelativeCamerePosition{rcp, 1.f}
    , relativeCamerePosition{rcp.x, 0.f, rcp.z, 1.f}
    , lookAtLocalPosition(relativeCamerePosition)
    , yTranslation{glm::translate(vec3(0.f, rcp.y, 0.f))}
    , mouseSensitivity_(.4f)
    , pitch{0.f}
    , yaw{0.f}
    , yawLimit{-75.f, 45.f}
    , pitchLimit{-40.f, 50.f}
    , mouseInactivityTimer{2.f}
{
    lookAtLocalPosition.z = 0;
}

RotateableRunState::~RotateableRunState()
{
}

void RotateableRunState::onEnter()
{
    yaw               = 0.f;
    pitch             = 0.f;
    mouseInactiveTime = 0.f;
    context.camera.setOnUpdate([this]() { cameraUpdate(); });
}

void RotateableRunState::onEnter(const MouseMoveEvent& event)
{
    updatePitchYaw(vec2(event.move.x, event.move.y));
}

void RotateableRunState::cameraUpdate()
{
    const vec2 mouseMove = CalcualteMouseMove() * mouseSensitivity_;
    mouseInactiveTime += context.displayManager.GetTime().deltaTime;

    if (mouseInactiveTime > mouseInactivityTimer)
    {
        if (thridPersonCameraComponent)
            thridPersonCameraComponent->pushEventToQueue(MouseInactivityEvent{yTranslation, pitch, yaw, 2.f, relativeCamerePosition, lookAtLocalPosition});
        return;
    }
    mouseControlledCamera(mouseMove);
}

const vec4& RotateableRunState::getRelativeCamerePosition() const
{
    return referenceRelativeCamerePosition;
}

const vec4& RotateableRunState::getLookAtPosition() const
{
    return lookAtLocalPosition;
}
vec2 RotateableRunState::CalcualteMouseMove()
{
    auto v = context.inputManager.CalcualteMouseMove();

    if (v.x > 0 or v.y > 0)
    {
        mouseInactiveTime = 0.f;
    }

    return vec2(v.x, v.y);
}
void RotateableRunState::lockPitch()
{
    if (pitch < pitchLimit.x)
        pitch = pitchLimit.x;
    if (pitch > pitchLimit.y)
        pitch = pitchLimit.y;
}

void RotateableRunState::updateYaw()
{
    if (yaw > 360.f)
    {
        yaw -= 360.f;
    }
}

void RotateableRunState::mouseControlledCamera(const vec2& mouseMove)
{
    updatePitchYaw(mouseMove);

    auto rotX = glm::normalize(glm::angleAxis(glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f)));
    auto rotY = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));

    auto parentWorldTransform = context.gameObject.GetWorldTransform().GetMatrix();
    parentWorldTransform      = parentWorldTransform * yTranslation * mat4_cast(rotY * rotX);

    auto worldCameraPosition = parentWorldTransform * relativeCamerePosition;
    context.camera.SetPosition(worldCameraPosition);

    auto lookAtPosition = parentWorldTransform * lookAtLocalPosition;
    context.camera.LookAt(lookAtPosition);
}

void RotateableRunState::updatePitchYaw(const vec2& mouseMove)
{
    yaw += mouseMove.x;
    pitch += mouseMove.y;

    lockPitch();
    updateYaw();
}
}  // namespace Camera
}  // namespace Components
}  // namespace GameEngine
