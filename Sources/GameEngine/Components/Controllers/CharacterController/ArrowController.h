#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Physics
{
struct CollisionContactInfo;
}

namespace Components
{
class Rigidbody;

class ArrowController : public BaseComponent
{
public:
    ArrowController(ComponentContext&, GameObject&);

    ArrowController& setDirection(const vec3&);
    void CleanUp() override;
    void ReqisterFunctions() override;

    void shoot();

private:
    void update();
    void onCollisionDetect(const Physics::CollisionContactInfo&);

private:
    Physics::CollisionSubId collisionSubId;
    Rigidbody* rigidbody;
    vec3 direction;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
