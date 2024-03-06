#include "AimController.h"

#include <Input/InputManager.h>

#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const mat4 matrixRotationOffset(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

AimController::AimController(GameObject& gameObject, Input::InputManager& inputManager, Animation::Joint& joint)
    : gameObject{gameObject}
    , inputManager{inputManager}
    , joint{joint}
    , thridPersonCameraComponent{gameObject.GetComponent<ThridPersonCameraComponent>()}
    , rigidbody{gameObject.GetComponent<Rigidbody>()}
    , camSensitive{0.2f}
    , boneRotatationLimit{0, 0}  // {-75.f, 45.f}
{
}

void AimController::enter()
{
    joint.ignoreParentRotation = true;
    joint.additionalUserMofiyTransform.set(matrixRotationOffset, mat4(1.0f));
    inputManager.CalcualteMouseMove();
}

void AimController::update()
{
    if (thridPersonCameraComponent)
    {
        auto [pitch, yaw] = thridPersonCameraComponent->getRotation();

        rotateCharacterIfBoneRotationExceeded(yaw);

        auto z = glm::normalize(glm::angleAxis(glm::radians(-pitch), glm::vec3(0.f, 0.f, 1.f)));
        auto y = glm::normalize(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
        joint.additionalUserMofiyTransform.set(glm::mat4_cast(y * z));
    }
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

void AimController::rotateCharacterIfBoneRotationExceeded(float& yaw)
{
    //rotateCharacter(yaw);
    rigidbody->SetRotation(glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f)));
    yaw = 0.f;
    return;

    //    // TO DO
    //    if (yaw < boneRotatationLimit.x)
    //    {
    //        //rotateCharacter(yaw - boneRotatationLimit.x);
    //        //rotateCharacter(90.f);
    //        // updateRotationOffset
    //        yaw = boneRotatationLimit.x;
    //    }

    //    if (yaw > boneRotatationLimit.y)
    //    {
    //       // rotateCharacter(yaw - boneRotatationLimit.y);
    //        // rotateCharacter(-90.f);
    //        // updateRotationOffset
    //        yaw = boneRotatationLimit.y;
    //    }
}

void AimController::rotateCharacter(float yaw)
{
    auto rotation = rigidbody->GetRotation() * glm::angleAxis(glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
    rigidbody->SetRotation(rotation);
}

vec2 AimController::calculateMouseMove()
{
    auto mouseMove = inputManager.CalcualteMouseMove();
    return vec2(static_cast<float>(mouseMove.x), static_cast<float>(mouseMove.y)) * camSensitive;
}

}  // namespace Components
}  // namespace GameEngine
