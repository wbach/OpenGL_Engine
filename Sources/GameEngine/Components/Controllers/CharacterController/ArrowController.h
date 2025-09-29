#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Physics
{
struct CollisionContactInfo;
}

namespace Components
{
class Rigidbody;
class ThridPersonCameraComponent;

class ArrowController : public BaseComponent
{
public:
    ArrowController(ComponentContext&, GameObject&);

    ArrowController& setCameraComponent(ThridPersonCameraComponent*);
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void shoot();

private:
    void update();
    Rigidbody* findCollidedRigidbody(uint32);
    void onCollisionDetect(const Physics::CollisionContactInfo&);
    void performCollision(uint32);
    void createPhysicsObject();
    void subscribeForCollisionDetection();

private:
    Physics::CollisionSubId collisionSubId;
    ThridPersonCameraComponent* thridPersonCameraComponent;
    Rigidbody* rigidbody;
    GameObject* physicArrowGameObject;

    vec3 lastPosition;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
