#pragma once
#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Physics/IPhysicsApi.h"

namespace GameEngine
{
namespace Components
{
class Rigidbody;
class CollisionShape : public BaseComponent
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

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
