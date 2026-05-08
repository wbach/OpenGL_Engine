#pragma once
#include "CollisionShape.h"
#include "GameEngine/Components/Component.h"
namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(CylinderShape, CollisionShape)
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
    CylinderShape(ComponentContext&, GameObject&);
    void ReqisterFunctions() override;
    void InitShape() override;

public:
    CylinderShape& SetSize(const vec3&);
    const vec3& GetSize() const;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
    static std::string name;
};
}  // namespace Components
}  // namespace GameEngine
