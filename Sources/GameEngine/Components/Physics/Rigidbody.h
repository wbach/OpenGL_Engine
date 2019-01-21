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
    Rigidbody& SetCollisionShape(ComponentsType shapeType);
    Rigidbody& SetVelocity(const vec3& velocity);
    float GetMass() const;
    bool IsStatic() const;
    ComponentsType GetCollisionShapeType() const;
    const vec3& GetVelocity() const;

private:
    void GetCollisionShape();
    bool isShapeTypeValid(ComponentsType shapeType);

private:
    float mass_;
    bool isStatic_;
    bool isInitilized_;
    CollisionShape* collisionShape_;
    uint32 rigidBodyId_;
    vec3 velocity_;
    ComponentsType shapeType_;

public:
    static ComponentsType type;
};
}  // namespace Components
}  // namespace GameEngine
