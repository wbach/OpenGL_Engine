#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class BoxShape : public CollisionShape
{
private:
    vec3 size;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR3F(size)
        FIELD_VECTOR3F(positionOffset)
    END_FIELDS()
    // clang-format on

public:
    BoxShape(ComponentContext&, GameObject&);
    void ReqisterFunctions() override;

public:
    BoxShape& SetSize(const vec3&);
    BoxShape& SetSize(float);
    const vec3& GetSize() const
    {
        return size;
    }

private:
    void InitShape() override;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static const std::string name;
};
}  // namespace Components
}  // namespace GameEngine
