#pragma once
#include "CollisionShape.h"
#include "GameEngine/Components/Component.h"
namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(BoxShape, CollisionShape)
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
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
    static const std::string name;
};
}  // namespace Components
}  // namespace GameEngine
