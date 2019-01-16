#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Physics/ShapeType.h"

namespace GameEngine
{
namespace Components
{
class CollisionShape;

class Rigidbody : public BaseComponent
{
public:
    Rigidbody(const ComponentContext& componentContext, GameObject& gameObject);
    ~Rigidbody();
    void OnStart();
    virtual void ReqisterFunctions() override;

public:
    Rigidbody& SetMass(float mass);
    Rigidbody& SetIsStatic(bool is);
    Rigidbody& SetCollisionShape(CollisionShape* collisionShape);
    Rigidbody& SetVelocity(const vec3& velocity);

private:
    float mass_;
    bool isStatic_;
    bool isInitilized_;
    CollisionShape* collisionShape_;
    uint32 rigidBodyId_;
    vec3 velocity_;

public:
    static ComponentsType type;
    ShapeType shapeType_;
};
}  // namespace Components
}  // namespace GameEngine
