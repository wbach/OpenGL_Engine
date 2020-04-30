#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class CollisionShape : public BaseComponent
{
public:
    CollisionShape(ComponentsType type, ComponentContext& componentContext, GameObject& gameObject);
    uint32 GetCollisionShapeId() const;

public:
    void SetPostionOffset(const vec3& position);
    const vec3& GetPositionOffset() const;

protected:
    uint32 collisionShapeId_;
    vec3 positionOffset_;
};
}  // namespace Components
}  // namespace GameEngine
