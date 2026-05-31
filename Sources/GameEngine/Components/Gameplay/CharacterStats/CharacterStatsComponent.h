#pragma once
#include <TreeNode.h>

#include <unordered_map>

#include "CharacterStats.h"
#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/ComponentCore.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Resources/File.h"
namespace GameEngine
{
namespace GUI
{
class Window;
class VerticalLayout;
class Button;
}  // namespace GUI

class GameState;

namespace Components
{
DECLARE_COMPONENT(CharacterStatsComponent)
{
public:
    Gothic::CharacterStats::General general;
    Gothic::CharacterStats::Offense offense;
    Gothic::CharacterStats::Attributes attributes;
    Gothic::CharacterStats::Protection protection;
    Gothic::CharacterStats::FightingSkills fightingSkills;
    Gothic::CharacterStats::ThievingSkills thievingSkills;
    Gothic::CharacterStats::SpecialSkills specialSkills;
    std::string faction;

public:
    // clang-format off
    BEGIN_FIELDS()
            FIELD_STRING(faction)
            FIELD_ENUM(general.guild)
            FIELD_UINT(general.level)
            FIELD_UINT(general.exp)
            FIELD_UINT(general.nextlvl)
            FIELD_UINT(general.skillPoints)

            // Offense
            FIELD_FLOAT(offense.meleeDamage.ptr())
            // FIELD_FLOAT(offense.meleeRange.ptr())
            // FIELD_FLOAT(offense.meleeAttackSpeed)
            // FIELD_FLOAT(offense.rangedDamage.ptr())
            // FIELD_FLOAT(offense.rangedAttackSpeed)
            // FIELD_FLOAT(offense.magicDamage.ptr())

            // Attributes
            FIELD_FLOAT(attributes.str.ptr())
            FIELD_FLOAT(attributes.dex.ptr())
            FIELD_FLOAT(attributes.currentLife)
            FIELD_FLOAT(attributes.maxLife.ptr())
            FIELD_FLOAT(attributes.currentMana)
            FIELD_FLOAT(attributes.maxMana.ptr())

            // Protection
            FIELD_FLOAT(protection.weapon.ptr())
            FIELD_FLOAT(protection.arrow.ptr())
            FIELD_FLOAT(protection.fire.ptr())
            FIELD_FLOAT(protection.magic.ptr())

            // Fighting Skills
            FIELD_FLOAT(fightingSkills.oneHand)
            FIELD_FLOAT(fightingSkills.twoHand)
            FIELD_FLOAT(fightingSkills.bow)
            FIELD_FLOAT(fightingSkills.crossbow)
            FIELD_FLOAT(fightingSkills.magic)

            // Thieving Skills
            FIELD_UINT(thievingSkills.openLocks)
            FIELD_UINT(thievingSkills.pickpocketing)
            FIELD_BOOL(thievingSkills.sneaking)

            // Special
            FIELD_BOOL(specialSkills.acrobatics)
    END_FIELDS()
    // clang-format on

public:
    CharacterStatsComponent(ComponentContext&, GameObject&);
    ~CharacterStatsComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
