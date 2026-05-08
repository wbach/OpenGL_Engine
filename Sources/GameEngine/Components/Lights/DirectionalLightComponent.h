#pragma once
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(DirectionalLightComponent)
{
public:
    bool isDayNightCycleControlled;
    Color color;
    float intensity;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_COLOR_RGB(color)
        FIELD_FLOAT(intensity)
        FIELD_BOOL(isDayNightCycleControlled)
    END_FIELDS()
    // clang-format on
public:
    DirectionalLightComponent(ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    void CleanUp() override;
    void Reload() override;

    void Init();
    vec3 GetDirection() const;

private:
    bool isSetInDayNightCycle;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
