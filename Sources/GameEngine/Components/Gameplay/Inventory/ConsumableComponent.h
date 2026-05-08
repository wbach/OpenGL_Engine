#pragma once
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
enum class EffectType : uint8_t
{
    None,
    InstantHeal,
    InstantMana,
    InstantStamina,
    BuffStrength,
    PermanentAddExp
};

namespace Components
{
DECLARE_COMPONENT(ConsumableComponent)
{
public:
    EffectType effect = EffectType::None;
    int power         = 0;
    float duration    = 0.0f;
    int charges       = 1;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_ENUM(effect)
        FIELD_INT(power)
        FIELD_FLOAT(duration)
        FIELD_INT(charges)
    END_FIELDS()
    // clang-format on

public:
    ConsumableComponent(ComponentContext&, GameObject&);
    ~ConsumableComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
