#include "ArrowController.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Physics/CapsuleShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
ArrowController::ArrowController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(ArrowController).hash_code(), componentContext, gameObject)
    , thridPersonCameraComponent{nullptr}
    , rigidbody{nullptr}
    , physicArrowGameObject{nullptr}
    , lastPosition{thisObject_.GetWorldTransform().GetPosition()}
{
}

ArrowController& ArrowController::setCameraComponent(ThridPersonCameraComponent* cameraComponent)
{
    thridPersonCameraComponent = cameraComponent;
    return *this;
}

void ArrowController::CleanUp()
{
    if (collisionSubId)
    {
        componentContext_.physicsApi_.celarCollisionCallback(collisionSubId);
        collisionSubId.reset();
    }
}

void ArrowController::ReqisterFunctions()
{
    //    RegisterFunction(FunctionType::OnStart,
    //                     [this]()
    //                     {
    //                         rigidbody = thisObject_.GetComponent<Rigidbody>();

    //                     });
    RegisterFunction(FunctionType::Update, std::bind(&ArrowController::update, this));
}

void ArrowController::shoot()
{
    if (not thridPersonCameraComponent)
        return;

    createPhysicsObject();
    subscribeForCollisionDetection();

    rigidbody->ApplyImpulse(thridPersonCameraComponent->getDirection() * 10.f);
    // rigidbody->SetVelocity(thridPersonCameraComponent->getDirection() * 10.f);

    lastPosition = thisObject_.GetWorldTransform().GetPosition();
}

Quaternion generateRotationFromDirection(const vec3& direction)
{
    // Calculate change-of-basis matrix
    glm::mat3 transform(1.f);

    if (direction.x == 0 && direction.z == 0)
    {
        if (direction.y < 0)  // rotate 180 degrees
        {
            transform =
                glm::mat3(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        // else if direction.y >= 0, leave transform as the identity matrix.
    }
    else
    {
        vec3 new_y = glm::normalize(direction);
        vec3 new_z = glm::normalize(glm::cross(new_y, vec3(0, 1, 0)));
        vec3 new_x = glm::normalize(glm::cross(new_y, new_z));

        transform = mat3(new_x, new_y, new_z);
    }
    return glm::quat_cast(transform);
}

void ArrowController::update()
{
    if (not rigidbody or not physicArrowGameObject)
        return;

    auto currentPos = thisObject_.GetWorldTransform().GetPosition();
    auto direction  = glm::normalize(currentPos - lastPosition);

    thisObject_.SetWorldPositionRotation(physicArrowGameObject->GetWorldTransform().GetPosition(),
                                         generateRotationFromDirection(direction));
    lastPosition = currentPos;
}

void ArrowController::onCollisionDetect(const Physics::CollisionContactInfo& info)
{
    if (not rigidbody)
    {
        WARNING_LOG("Something went wrong");
        return;
    }

    DEBUG_LOG("Collision detected rigidbodyId=" + std::to_string(rigidbody->GetId()) + "(" +
              std::to_string(info.rigidbodyId1) + ") with rigidbodyId=" + std::to_string(info.rigidbodyId2) +
              ", Oncollision p1 : " + std::to_string(info.pos1) + ", p2 : " + std::to_string(info.pos2));

    auto rigidbodies = componentContext_.componentController_.GetAllComonentsOfType<Rigidbody>();

    auto iter = std::find_if(rigidbodies.begin(), rigidbodies.end(),
                             [id = info.rigidbodyId2](const auto& rigidbody) { return id == rigidbody->GetId(); });

    if (iter != rigidbodies.end())
    {
        if ((*iter)->GetParentGameObject().GetName() != "Player")  // TO DO : check tag
        {
            componentContext_.physicsApi_.celarCollisionCallback(collisionSubId);
            collisionSubId.reset();

            physicArrowGameObject->RemoveComponent<Rigidbody>();

            // componentContext_.scene_.RemoveGameObject(physicArrowGameObject->GetId());
            rigidbody             = nullptr;
            physicArrowGameObject = nullptr;
        }
    }
}

void ArrowController::createPhysicsObject()
{
    auto p = componentContext_.scene_.CreateGameObject(thisObject_.GetName() + "_Rigidbody");

    p->AddComponent<Components::CapsuleShape>().SetHeight(1.75f).SetRadius(0.05f);
    rigidbody = &p->AddComponent<Rigidbody>();
    rigidbody->SetMass(1.f);
    rigidbody->SetNoContactResponse(true);
    p->SetWorldMatrix(thisObject_.GetWorldTransform().GetMatrix());
    physicArrowGameObject = p.get();
    componentContext_.scene_.AddGameObject(std::move(p));
}

void ArrowController::subscribeForCollisionDetection()
{
    collisionSubId = componentContext_.physicsApi_.setCollisionCallback(
        rigidbody->GetId(), [this](const auto& info) { onCollisionDetect(info); });
}

void ArrowController::registerReadFunctions()
{
}

void ArrowController::write(TreeNode&) const
{
}

}  // namespace Components
}  // namespace GameEngine
