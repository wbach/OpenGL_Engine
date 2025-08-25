#include "ArrowController.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
namespace Components
{
ArrowController::ArrowController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent("ArrowController", componentContext, gameObject)
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
    RegisterFunction(FunctionType::Update, std::bind(&ArrowController::update, this));
}

void ArrowController::shoot()
{
    if (not thridPersonCameraComponent)
        return;

    createPhysicsObject();
    //  subscribeForCollisionDetection(); To remove if all will be ok

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
    if (not physicArrowGameObject)
        return;

    auto currentPos = thisObject_.GetWorldTransform().GetPosition();
    auto direction  = generateRotationFromDirection(glm::normalize(currentPos - lastPosition));

    thisObject_.SetWorldPositionRotation(physicArrowGameObject->GetWorldTransform().GetPosition(), direction);

    auto rayTestResult = componentContext_.physicsApi_.RayTest(lastPosition, currentPos);

    if (rayTestResult)
    {
        performCollision(rayTestResult->rigidbodyId);

        vec3 offset(0, 0.6f, 0);
        auto rotatedOffset = direction * offset;
        thisObject_.SetWorldPosition( rayTestResult->pointWorld - rotatedOffset);  // lastPosition, rayTestResult->pointWorld but last make better result?
    }
    lastPosition = currentPos;
}

void ArrowController::onCollisionDetect(const Physics::CollisionContactInfo& info)
{
    //    DEBUG_LOG("Collision detected rigidbodyId=" + std::to_string(rigidbody->GetId()) + "(" +
    //              std::to_string(info.rigidbodyId1) + ") with rigidbodyId=" + std::to_string(info.rigidbodyId2) +
    //              ", Oncollision p1 : " + std::to_string(info.pos1) + ", p2 : " + std::to_string(info.pos2));

    performCollision(info.rigidbodyId2);
}

void ArrowController::performCollision(uint32 rigidbodyId)
{
    auto rigidbody = findCollidedRigidbody(rigidbodyId);
    if (not rigidbody)
    {
        WARNING_LOG("Rigidbody not found : " + std::to_string(rigidbodyId));
        return;
    }

    if (rigidbody->GetParentGameObject().GetName() != "Player")  // TO DO : check tag
    {
        DEBUG_LOG("Collision detected with " + rigidbody->GetParentGameObject().GetName());
        componentContext_.physicsApi_.celarCollisionCallback(collisionSubId);
        collisionSubId.reset();

        physicArrowGameObject->RemoveComponent<Rigidbody>();
        // componentContext_.scene_.RemoveGameObject(physicArrowGameObject->GetId());
        physicArrowGameObject = nullptr;
    }
}

void ArrowController::createPhysicsObject()
{
    auto p = componentContext_.scene_.CreateGameObject(thisObject_.GetName() + "_Rigidbody");

    p->AddComponent<Components::SphereShape>().SetRadius(0.1f);
    rigidbody = &p->AddComponent<Rigidbody>();
    rigidbody->SetMass(0.5f);
    rigidbody->SetNoContactResponse(true);
    p->SetWorldMatrix(thisObject_.GetWorldTransform().GetMatrix());
    physicArrowGameObject = p.get();
    componentContext_.scene_.AddGameObject(std::move(p));
}

void ArrowController::subscribeForCollisionDetection()
{
    Physics::CollisionDetection cd{.action   = Physics::CollisionDetection::Action::on,
                                   .type     = Physics::CollisionDetection::Type::single,
                                   .callback = [this](const auto& infos)
                                   {
                                       for (const auto& info : infos)
                                       {
                                           onCollisionDetect(info);
                                       }
                                   }};

    collisionSubId = componentContext_.physicsApi_.setCollisionCallback(
        rigidbody->GetId(), cd);
}

Rigidbody* ArrowController::findCollidedRigidbody(uint32 rigidbodyId)
{
    auto rigidbodies = componentContext_.componentController_.GetAllComonentsOfType<Rigidbody>();

    auto iter = std::find_if(rigidbodies.begin(), rigidbodies.end(),
                             [id = rigidbodyId](const auto& rigidbody) { return id == rigidbody->GetId(); });

    return (iter != rigidbodies.end()) ? *iter : nullptr;
}

void ArrowController::registerReadFunctions()
{
}

void ArrowController::write(TreeNode&) const
{
}

}  // namespace Components
}  // namespace GameEngine
