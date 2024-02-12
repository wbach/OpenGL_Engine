#include "AimController.h"

#include <Input/InputManager.h>

#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const mat4 matrixRotationOffset(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

AimController::AimController(Input::InputManager& inputManager, Animation::Joint& joint,
                             ThridPersonCameraComponent* thridPersonCameraComponent)
    : inputManager{inputManager}
    , joint{joint}
    , thridPersonCameraComponent{thridPersonCameraComponent}
    , camSensitive{0.2f}
    , yawLimit{-75.f, 45.f}
    , pitchLimit{-40.f, 50.f}
{
}

void AimController::enter()
{
    joint.ignoreParentRotation               = true;
    joint.additionalUserMofiyTransform.pitch = 0.f;
    joint.additionalUserMofiyTransform.yaw   = 0.f;
    joint.additionalUserMofiyTransform.set(matrixRotationOffset, mat4(1.0f));
    inputManager.CalcualteMouseMove();
}

void AimController::update()
{
    auto& yaw   = joint.additionalUserMofiyTransform.yaw;
    auto& pitch = joint.additionalUserMofiyTransform.pitch;

    auto mouseMove = calculateMouseMove();

    if (not thridPersonCameraComponent->isAimReady())
        return;

    yaw -= mouseMove.x;
    pitch -= mouseMove.y;

    LockYaw(yaw);
    LockPitch(pitch);

    auto z = glm::normalize(glm::angleAxis(glm::radians(pitch), glm::vec3(0.f, 0.f, 1.f)));
    auto y = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
    joint.additionalUserMofiyTransform.set(glm::mat4_cast(y * z));
}

void AimController::reset()
{
    joint.additionalUserMofiyTransform = Animation::Joint::AdditionalUserMofiyTransform{};
    joint.ignoreParentRotation         = false;
}

const Animation::Joint& AimController::getJoint() const
{
    return joint;
}

void AimController::LockPitch(float& pitch)
{
    if (pitch < pitchLimit.x)
        pitch = pitchLimit.x;
    if (pitch > pitchLimit.y)
        pitch = pitchLimit.y;
}

void AimController::LockYaw(float& yaw)
{
    if (yaw < yawLimit.x)
        yaw = yawLimit.x;
    if (yaw > yawLimit.y)
        yaw = yawLimit.y;
}

vec2 AimController::calculateMouseMove()
{
    auto mouseMove = inputManager.CalcualteMouseMove();
    return vec2(static_cast<float>(mouseMove.x), static_cast<float>(mouseMove.y)) * camSensitive;
}

}  // namespace Components
}  // namespace GameEngine
