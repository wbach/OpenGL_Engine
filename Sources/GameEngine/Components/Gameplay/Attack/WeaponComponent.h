#pragma once
#include <vector>

#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(WeaponComponent)
{
public:
    float radius{.1f};
    std::vector<vec3> socketOffsets;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FLOAT(radius)
        FIELD_VECTOR_OF_VECTOR3F(socketOffsets)
    END_FIELDS()
    // clang-format on

public:
    WeaponComponent(ComponentContext&, GameObject&);
    ~WeaponComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    std::vector<vec3> GetWorldSocketPositions() const;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
