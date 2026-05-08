#pragma once
#include "CollisionShape.h"
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(SphereShape, CollisionShape)
{
public:
    float radius;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(radius)
        FIELD_VECTOR3F(positionOffset)
    END_FIELDS()
    // clang-format on
public:
    SphereShape(ComponentContext& componentContext, GameObject& gameObject);
    void ReqisterFunctions() override;
    void InitShape() override;

public:
    SphereShape& SetRadius(float size);
    inline float GetRadius() const;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
    static std::string name;
};
float SphereShape::GetRadius() const
{
    return radius;
}
}  // namespace Components
}  // namespace GameEngine
