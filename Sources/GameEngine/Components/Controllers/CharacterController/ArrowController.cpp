#include "ArrowController.h"

#include <Logger/Log.h>

#include "GameEngine/Components/Camera/ThridPersonCamera/ThridPersonCameraComponent.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Physics/CollisionContactInfo.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
ArrowController::ArrowController(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(typeid(ArrowController).hash_code(), componentContext, gameObject)
    , thridPersonCameraComponent{nullptr}
    , rigidbody{nullptr}
    , direction(VECTOR_FORWARD)
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
    if (not rigidbody)
    {
        rigidbody = thisObject_.GetComponent<Rigidbody>();
        if (rigidbody)
        {
            DEBUG_LOG("setCollisionCallback");
            collisionSubId = componentContext_.physicsApi_.setCollisionCallback(
                rigidbody->GetId(), [this](const auto& info) { onCollisionDetect(info); });
        }
    }
    if (rigidbody and thridPersonCameraComponent)
    {
       // rigidbody->InputParams().angularFactor_ = vec3(0);
        //rigidbody->ApplyImpulse(thridPersonCameraComponent->getDirection() * 10.f);
        rigidbody->SetVelocity(thridPersonCameraComponent->getDirection() * 10.f);
    }
}

void ArrowController::update()
{
    // DEBUG_LOG("update");
    if (not rigidbody)
        return;

    //    const auto moveSpeed = vec3(1.0f);

    //    auto targetVelocity = rigidbody->GetRotation() * vec3(VECTOR_UP) * moveSpeed;
    //    rigidbody->SetVelocity(targetVelocity);
    //    //DEBUG_LOG("targetVelocity : " + std::to_string(targetVelocity));
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
            thisObject_.RemoveComponent<Rigidbody>();
            rigidbody = nullptr;

            componentContext_.physicsApi_.celarCollisionCallback(collisionSubId);
            collisionSubId.reset();
        }
    }
}

void ArrowController::registerReadFunctions()
{
}

void ArrowController::write(TreeNode&) const
{
}

}  // namespace Components
}  // namespace GameEngine
