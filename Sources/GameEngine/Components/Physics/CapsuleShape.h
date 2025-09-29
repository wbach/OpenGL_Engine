#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class CapsuleShape : public CollisionShape
{
public:
    float height;
    float radius;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(height)
        FIELD_FLOAT(radius)
        FIELD_VECTOR3F(positionOffset)
    END_FIELDS()
    // clang-format on

public:
    CapsuleShape(ComponentContext& componentContext, GameObject& gameObject);
    void ReqisterFunctions() override;
    void InitShape() override;

public:
    CapsuleShape& SetHeight(float h);
    CapsuleShape& SetRadius(float r);
    inline float GetHeight() const;
    inline float GetRadius() const;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
};

float CapsuleShape::GetHeight() const
{
    return height;
}
inline float CapsuleShape::GetRadius() const
{
    return radius;
}
}  // namespace Components
}  // namespace GameEngine
