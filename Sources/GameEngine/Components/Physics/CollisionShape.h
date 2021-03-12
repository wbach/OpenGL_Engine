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

    virtual void setScale(const vec3&);
    std::optional<uint32> GetCollisionShapeId() const;
    void CleanUp() override;

public:
    void SetPostionOffset(const vec3& position);
    const vec3& GetPositionOffset() const;

protected:
    std::optional<uint32> collisionShapeId_;
    vec3 positionOffset_;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
