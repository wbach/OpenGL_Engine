#pragma once
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(PointLightComponent)
{
public:
    Color color;
    float intensity;
    float range;
    float falloffExponent;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_COLOR_RGB(color)
        FIELD_FLOAT(intensity)
        FIELD_FLOAT(range)
        FIELD_FLOAT(falloffExponent)
    END_FIELDS()
    // clang-format on

public:
    PointLightComponent(ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    void CleanUp() override;
    void Reload() override;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
