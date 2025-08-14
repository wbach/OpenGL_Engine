#pragma once
#include "GameEngine/Physics/IPhysicsApi.h"
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class CollisionShape : public BaseComponent
{
public:
    CollisionShape(size_t, ComponentContext&, GameObject&);

    virtual void setScale(const vec3&);
    const Physics::ShapeId& GetCollisionShapeId() const;
    void CleanUp() override;

public:
    void SetPostionOffset(const vec3&);
    const vec3& GetPositionOffset() const;

protected:
    Physics::ShapeId collisionShapeId_;
    vec3 positionOffset;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
