#pragma once
#include "GameEngine/Components/Component.h"

namespace GameEngine
{
namespace Components
{
DECLARE_COMPONENT(CombatStatsComponent)
{
public:
    int damage        = 0;
    float attackSpeed = 1.0f;
    float range       = 1.0f;

    int physicalDefense = 0;
    int magicDefense    = 0;
    int fireResistance  = 0;

    int hpBonus      = 0;
    int manaBonus    = 0;
    int staminaBonus = 0;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_INT(damage)
        FIELD_FLOAT(attackSpeed)
        FIELD_FLOAT(range)
        FIELD_INT(physicalDefense)
        FIELD_INT(magicDefense)
        FIELD_INT(fireResistance)
        FIELD_INT(hpBonus)
        FIELD_INT(manaBonus)
        FIELD_INT(staminaBonus)
    END_FIELDS()
    // clang-format on

public:
    CombatStatsComponent(ComponentContext&, GameObject&);
    ~CombatStatsComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
