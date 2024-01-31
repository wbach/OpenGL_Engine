#include "AimController.h"

#include <Input/InputManager.h>

#include "GameEngine/Animations/Joint.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const mat4 matrixRotationOffset(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

AimController::AimController(Input::InputManager& inputManager, Animation::Joint& joint)
    : inputManager{inputManager}
    , joint{joint}
    , pitch{0.f}
    , yaw{0.f}
    , camSensitive{0.2f}
    , yawLimit{-75.f, 45.f}
    , pitchLimit{-40.f, 50.f}
{
}

void AimController::enter()
{
    joint.ignoreParentRotation         = true;
    joint.additionalUserMofiyTransform = matrixRotationOffset;

    yaw   = 0.f;
    pitch = 0.f;
    inputManager.CalcualteMouseMove();
}

void AimController::update()
{
    auto mouseMove = calculateMouseMove();
    yaw -= mouseMove.x;
    pitch -= mouseMove.y;

    LockYaw();
    LockPitch();

    joint.additionalRotations.x.value_ = glm::normalize(glm::angleAxis(glm::radians(-pitch), glm::vec3(1.f, 0.f, 0.f)));
    joint.additionalRotations.y.value_ = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
    joint.additionalRotations.z.value_ = glm::normalize(glm::angleAxis(glm::radians(pitch), glm::vec3(0.f, 0.f, 1.f)));

    joint.additionalUserMofiyTransform =
        matrixRotationOffset * glm::mat4_cast(joint.additionalRotations.y.value_ * joint.additionalRotations.z.value_);
}

void AimController::reset()
{
    pitch = 0.0f;
    yaw   = 0.0f;

    joint.additionalUserMofiyTransform = mat4(1.f);
    joint.additionalRotations          = Animation::Joint::AdditionalRotations{};
    joint.ignoreParentRotation         = false;
}

const Animation::Joint &AimController::getJoint() const
{
    return joint;
}

void AimController::LockPitch()
{
    if (pitch < pitchLimit.x)
        pitch = pitchLimit.x;
    if (pitch > pitchLimit.y)
        pitch = pitchLimit.y;
}

void AimController::LockYaw()
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
