#pragma once
#include "GameEngine/Components/ComponentCore.h"
#include "GameEngine/Physics/PhysicsApiTypes.h"

namespace GameEngine
{
namespace Components
{
class Rigidbody;
class CollisionShape : public ComponentCore
{
public:
    CollisionShape(const ComponentType&, ComponentContext&, GameObject&);

    virtual void setScale(const vec3&);
    const Physics::ShapeId& GetCollisionShapeId() const;
    void CleanUp() override;
    void Disconnect();
    void Connect(Rigidbody&);
    void Reload() override;

    virtual void InitShape() = 0;
    void RemoveShape();

public:
    void SetPostionOffset(const vec3&);
    const vec3& GetPositionOffset() const;

protected:
    Physics::ShapeId collisionShapeId_;
    vec3 positionOffset;

    Rigidbody* connectedRigidbody;

};
}  // namespace Components
}  // namespace GameEngine
