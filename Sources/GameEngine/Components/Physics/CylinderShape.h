#pragma once
#include "CollisionShape.h"

namespace GameEngine
{
namespace Components
{
class CylinderShape : public CollisionShape
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
    void OnAwake();

public:
    CylinderShape& SetSize(const vec3&);
    const vec3& GetSize() const { return size; }

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
    static std::string name;
};
}  // namespace Components
}  // namespace GameEngine
