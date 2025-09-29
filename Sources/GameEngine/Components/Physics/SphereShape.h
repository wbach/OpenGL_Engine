#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class SphereShape : public CollisionShape
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
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static std::string name;
};
float SphereShape::GetRadius() const
{
    return radius;
}
}  // namespace Components
}  // namespace GameEngine
