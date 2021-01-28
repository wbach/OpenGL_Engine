#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class CollisionShape : public BaseComponent
{
public:
    CollisionShape(size_t type, ComponentContext& componentContext, GameObject& gameObject);
    uint32 GetCollisionShapeId() const;

public:
    void SetPostionOffset(const vec3& position);
    const vec3& GetPositionOffset() const;

protected:
    uint32 collisionShapeId_;
    vec3 positionOffset_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
