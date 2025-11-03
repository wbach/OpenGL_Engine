#include "AimController.h"

#include <Input/InputManager.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>

#include "GameEngine/Animations/Joint.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Controllers/CharacterController/ArrowController.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
namespace Components
{
namespace
{
const mat4 matrixRotationOffset(glm::rotate(mat4(1.0f), ToRadians(-90.f), glm::vec3(0.f, 1.f, 0.f)));
}  // namespace

AimController::AimController(Scene& scene, GameObject& gameObject, Input::InputManager& inputManager, Animation::Joint& joint,
                             const std::string& drawArrowAnimName)
    : inputManager{inputManager}
    , joint{joint}
    , scene{scene}
    , gameObject{gameObject}
    , thridPersonCameraComponent{gameObject.GetComponent<ThridPersonCameraComponent>()}
    , rigidbody{gameObject.GetComponent<Rigidbody>()}
    , animator{gameObject.GetComponent<Animator>()}
    , camSensitive{0.2f}
    , boneRotatationLimit{0, 0}  // {-75.f, 45.f}
    , drawArrowAnimName{drawArrowAnimName}
{
}

AimController::~AimController()
{
}

void AimController::enter()
{
    joint.ignoreParentRotation = true;
    joint.additionalUserMofiyTransform.set(matrixRotationOffset, mat4(1.0f));
    inputManager.CalcualteMouseMove();

    if (not subscribeForDrawArrowAnimationFrame)
    {
        subscribeForDrawArrowAnimationFrame = animator->SubscribeForAnimationFrame(
            drawArrowAnimName, [this]() { createArrowObject(); }, Animation::FrameIndex{10});
    }
}

void AimController::reload()
{
    // createArrowObject();
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

    if (arrowGameObject)
    {
        scene.RemoveGameObject(*arrowGameObject);
        arrowGameObject = nullptr;
    }

    if (subscribeForDrawArrowAnimationFrame)
    {
        animator->UnSubscribeForAnimationFrame(*subscribeForDrawArrowAnimationFrame);
        subscribeForDrawArrowAnimationFrame.reset();
    }
}

void AimController::shoot()
{
    if (arrowGameObject)
    {
        if (updateJointBufferSubId_)
            animator->unSubscribeForPoseUpdateBuffer(*updateJointBufferSubId_);
        updateJointBufferSubId_.reset();
        jointPoseUpdater.reset();

        scene.RemoveParent(*arrowGameObject);
        arrowGameObject->GetComponent<Components::ArrowController>()->shoot();
        arrowGameObject = nullptr;
    }
}

const Animation::Joint& AimController::getJoint() const
{
    return joint;
}

void AimController::rotateCharacterIfBoneRotationExceeded(float& yaw)
{
    // rotateCharacter(yaw);
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

void AimController::createArrowObject()
{
    if (arrowGameObject)
        return;

    auto path = Utils::GetAbsolutePath(EngineConf.files.getDataPath()) / "mixamo.com" / "arrow2.obj";
    if (not Utils::CheckFileExist(path))
    {
        LOG_WARN << "arrow model path not found. Path: \"" << path << "\"";
        return;
    }

    if (not thridPersonCameraComponent)
    {
        return;
    }

    // auto id = arrowIds.getId();
    //  auto arrowObject = scene.CreateGameObject(gameObject.GetName() + "_Arrow_" + std::to_string(id));
    auto arrowObject = scene.CreateGameObject("_Arrow_");
    arrowObject->AddComponent<Components::RendererComponent>().AddModel(path);
    arrowObject->AddComponent<Components::ArrowController>().setCameraComponent(thridPersonCameraComponent);
    arrowGameObject = arrowObject.get();
    scene.AddGameObject(gameObject, std::move(arrowObject));
    arrowGameObject->SetWorldScale(vec3(0.75f));
    auto joint = animator->GetJoint("mixamorig:RightHand");
    if (joint)
    {
        auto rendererCopmponent = gameObject.GetComponent<RendererComponent>();

        if (not rendererCopmponent)
        {
            LOG_WARN << "RendererComponent not found";
            return;
        }

        auto model = rendererCopmponent->GetModelWrapper().Get();

        if (not model or model->GetMeshes().empty())
        {
            LOG_WARN << "Mesh not found";
            return;
        }
        auto meshTransform = model->GetMeshes().front().GetMeshTransform();

        jointPoseUpdater        = std::make_unique<Components::JointPoseUpdater>(*arrowGameObject, joint, meshTransform);
        updateJointBufferSubId_ = animator->subscribeForPoseBufferUpdate(
            [this]()
            {
                if (jointPoseUpdater)
                    jointPoseUpdater->updateGameObjectTransform();
            });
    }
    LOG_DEBUG << "Add to gameobject: " << gameObject.GetName();
}

vec2 AimController::calculateMouseMove()
{
    auto mouseMove = inputManager.CalcualteMouseMove();
    return vec2(static_cast<float>(mouseMove.x), static_cast<float>(mouseMove.y)) * camSensitive;
}

}  // namespace Components
}  // namespace GameEngine
