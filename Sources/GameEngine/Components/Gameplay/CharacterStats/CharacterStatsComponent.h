#pragma once
#include <TreeNode.h>

#include <unordered_map>

#include "CharacterStats.h"
#include "GameEngine/Components/Component.h"
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
    Gothic::CharacterStats::Attributes attributes;
    Gothic::CharacterStats::Protection protection;
    Gothic::CharacterStats::FightingSkills fightingSkills;
    Gothic::CharacterStats::ThievingSkills thievingSkills;
    Gothic::CharacterStats::SpecialSkills specialSkills;

public:
    // clang-format off
    BEGIN_FIELDS()
            FIELD_ENUM(general.guild)
            FIELD_UINT(general.level)
            FIELD_UINT(general.exp)
            FIELD_UINT(general.nextlvl)
            FIELD_UINT(general.skillPoints)

            // Attributes
            FIELD_UINT(attributes.str)
            FIELD_UINT(attributes.dex)
            FIELD_VECTOR2F(attributes.mana)
            FIELD_VECTOR2F(attributes.life)

            // Protection
            FIELD_UINT(protection.weapon)
            FIELD_UINT(protection.arrow)
            FIELD_UINT(protection.fire)
            FIELD_UINT(protection.magic)

            // Fighting Skills
            FIELD_UINT(fightingSkills.oneHand)
            FIELD_UINT(fightingSkills.twoHand)
            FIELD_UINT(fightingSkills.bow)
            FIELD_UINT(fightingSkills.crossbow)
            FIELD_UINT(fightingSkills.magic)

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
