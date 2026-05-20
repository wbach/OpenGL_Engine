#pragma once
#include <functional>
#include <vector>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/ComponentCore.h"
#include "GameEngine/Components/Gameplay/CharacterStats/ModifiableStat.h"
#include "GameEngine/Components/VectorOfCustomStructure.h"

namespace GameEngine
{
namespace Components
{
enum class TargetStat : uint8_t
{
    Strength,
    Dexterity,
    MaxMana,
    MaxLife,
    ProjectionWeapon,
    ProjectionArrow,
    ProjectionFire,
    ProjectionMagic,
    MeleDamage,
    MeleeAttackSpeed,
    RangedDamage,
    RangedAttackSpeed,
    MagicDamage,
    CastSpeed
};
struct ItemStatModifier
{
    TargetStat target;
    ModifierType type;
    float value{0.f};
};

DECLARE_COMPONENT(CombatStatsComponent)
{
public:
    VectorOfCustomStructure modifiersFields;

    // clang-format off
    BEGIN_FIELDS()
        FIELD_VECTOR_OF_CUSTOM(modifiersFields)
    END_FIELDS()
    // clang-format on

public:
    CombatStatsComponent(ComponentContext&, GameObject&);
    ~CombatStatsComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    std::vector<ItemStatModifier> modifiers;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
