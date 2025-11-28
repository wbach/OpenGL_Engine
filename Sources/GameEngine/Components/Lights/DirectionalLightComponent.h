#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class DirectionalLightComponent : public BaseComponent
{
public:
    Color color;
    float intensity;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_COLOR_RGB(color)
        FIELD_FLOAT(intensity)
    END_FIELDS()
    // clang-format on
public:
    DirectionalLightComponent(ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    void CleanUp() override;
    void Reload() override;

    vec3 GetDirection() const;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
