#pragma once
#include "GameEngine/Components/BaseComponent.h"

namespace GameEngine
{
namespace Components
{
class SpotLightComponent : public BaseComponent
{
public:
    Color color;
    float intensity;
    vec3 direction;
    float range;
    float falloffExponent;
    float innerCutoff;
    float outerCutoff;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_COLOR_RGB(color)
        FIELD_FLOAT(intensity)
        FIELD_VECTOR3F(direction)
        FIELD_FLOAT(range)
        FIELD_FLOAT(falloffExponent)
        FIELD_FLOAT(innerCutoff)
        FIELD_FLOAT(outerCutoff)
    END_FIELDS()
    // clang-format on
public:
    SpotLightComponent(ComponentContext&, GameObject&);

    void ReqisterFunctions() override;
    void CleanUp() override;
    void Reload() override;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
