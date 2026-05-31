#include "CharacterStatsComponent.h"

#include <Input/InputManager.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <string>
#include <utility>

#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Renderers/GUI/Button/Button.h"
#include "GameEngine/Renderers/GUI/ElementReader.h"
#include "GameEngine/Renderers/GUI/ElementWriter.h"
#include "GameEngine/Renderers/GUI/IElementFactory.h"
#include "GameEngine/Renderers/GUI/IGuiRenderer.h"
#include "GameEngine/Renderers/GUI/Layout/HorizontalLayout.h"
#include "GameEngine/Renderers/GUI/Layout/VerticalLayout.h"
#include "GameEngine/Renderers/GUI/Manager.h"
#include "GameEngine/Renderers/GUI/Text/MultiLineText.h"
#include "GameEngine/Renderers/GUI/Text/Text.h"
#include "GameEngine/Renderers/GUI/Transform.h"
#include "GameEngine/Renderers/GUI/Window/Window.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Utils.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace
{
// General
constexpr char CSTR_GENERAL_FACTION[]      = "general.faction";
constexpr char CSTR_GENERAL_GUILD[]        = "general.guild";
constexpr char CSTR_GENERAL_LEVEL[]        = "general.level";
constexpr char CSTR_GENERAL_EXP[]          = "general.exp";
constexpr char CSTR_GENERAL_NEXTLVL[]      = "general.nextlvl";
constexpr char CSTR_GENERAL_SKILL_POINTS[] = "general.skillPoints";

// Offense
constexpr char CSTR_OFFENSE_MELE_DMG[] = "offense.meledmg";

// Attributes
constexpr char CSTR_ATTR_STR[]      = "attributes.str";
constexpr char CSTR_ATTR_DEX[]      = "attributes.dex";
constexpr char CSTR_ATTR_MANA[]     = "attributes.mana";
constexpr char CSTR_ATTR_MANA_MAX[] = "attributes.manaMax";
constexpr char CSTR_ATTR_LIFE[]     = "attributes.life";
constexpr char CSTR_ATTR_LIFE_MAX[] = "attributes.lifeMax";

// Protection
constexpr char CSTR_PROT_WEAPON[] = "protection.weapon";
constexpr char CSTR_PROT_ARROW[]  = "protection.arrow";
constexpr char CSTR_PROT_FIRE[]   = "protection.fire";
constexpr char CSTR_PROT_MAGIC[]  = "protection.magic";

// Fighting Skills
constexpr char CSTR_FIGHT_1H[]   = "fighting.oneHand";
constexpr char CSTR_FIGHT_2H[]   = "fighting.twoHand";
constexpr char CSTR_FIGHT_BOW[]  = "fighting.bow";
constexpr char CSTR_FIGHT_CBOW[] = "fighting.crossbow";
constexpr char CSTR_FIGHT_MAG[]  = "fighting.magic";

// Thieving Skills
constexpr char CSTR_THIEF_LOCKS[]      = "thieving.openLocks";
constexpr char CSTR_THIEF_PICKPOCKET[] = "thieving.pickpocketing";
constexpr char CSTR_THIEF_SNEAK[]      = "thieving.sneaking";

// Special
constexpr char CSTR_SPEC_ACROBATICS[] = "special.acrobatics";
}  // namespace
namespace Components
{
REGISTER_COMPONENT(CharacterStatsComponent)

CharacterStatsComponent::CharacterStatsComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}
CharacterStatsComponent::~CharacterStatsComponent()
{
}
void CharacterStatsComponent::CleanUp()
{
}
void CharacterStatsComponent::Reload()
{
}
void CharacterStatsComponent::ReqisterFunctions()
{
}
void CharacterStatsComponent::read(const TreeNode& input)
{
    // General
    ::Read(input.getChild(CSTR_GENERAL_FACTION), faction);
    if (not faction.empty())
        general.faction = Utils::HashString(faction);

    ::Read(input.getChild(CSTR_GENERAL_GUILD), general.guild);
    ::Read(input.getChild(CSTR_GENERAL_LEVEL), general.level);
    ::Read(input.getChild(CSTR_GENERAL_EXP), general.exp);
    ::Read(input.getChild(CSTR_GENERAL_NEXTLVL), general.nextlvl);
    ::Read(input.getChild(CSTR_GENERAL_SKILL_POINTS), general.skillPoints);

    // Offense
    ::Read(input.getChild(CSTR_OFFENSE_MELE_DMG), offense.meleeDamage);

    // Attributes
    ::Read(input.getChild(CSTR_ATTR_DEX), attributes.dex);
    ::Read(input.getChild(CSTR_ATTR_STR), attributes.str);
    ::Read(input.getChild(CSTR_ATTR_MANA), attributes.currentMana);
    ::Read(input.getChild(CSTR_ATTR_MANA_MAX), attributes.currentMana);
    ::Read(input.getChild(CSTR_ATTR_LIFE), attributes.currentLife);
    ::Read(input.getChild(CSTR_ATTR_LIFE_MAX), attributes.maxLife);

    // Protection
    ::Read(input.getChild(CSTR_PROT_WEAPON), protection.weapon);
    ::Read(input.getChild(CSTR_PROT_ARROW), protection.arrow);
    ::Read(input.getChild(CSTR_PROT_FIRE), protection.fire);
    ::Read(input.getChild(CSTR_PROT_MAGIC), protection.magic);

    // Fighting Skills
    ::Read(input.getChild(CSTR_FIGHT_1H), fightingSkills.oneHand);
    ::Read(input.getChild(CSTR_FIGHT_2H), fightingSkills.twoHand);
    ::Read(input.getChild(CSTR_FIGHT_BOW), fightingSkills.bow);
    ::Read(input.getChild(CSTR_FIGHT_CBOW), fightingSkills.crossbow);
    ::Read(input.getChild(CSTR_FIGHT_MAG), fightingSkills.magic);

    // Thieving Skills
    ::Read(input.getChild(CSTR_THIEF_LOCKS), thievingSkills.openLocks);
    ::Read(input.getChild(CSTR_THIEF_PICKPOCKET), thievingSkills.pickpocketing);
    ::Read(input.getChild(CSTR_THIEF_SNEAK), thievingSkills.sneaking);

    // Special
    ::Read(input.getChild(CSTR_SPEC_ACROBATICS), specialSkills.acrobatics);
}

void CharacterStatsComponent::write(TreeNode& node) const
{
    // General
    ::write(node.addChild(CSTR_GENERAL_FACTION), faction);
    ::write(node.addChild(CSTR_GENERAL_GUILD), general.guild);
    ::write(node.addChild(CSTR_GENERAL_LEVEL), general.level);
    ::write(node.addChild(CSTR_GENERAL_EXP), general.exp);
    ::write(node.addChild(CSTR_GENERAL_NEXTLVL), general.nextlvl);
    ::write(node.addChild(CSTR_GENERAL_SKILL_POINTS), general.skillPoints);

    // Attributes
    ::write(node.addChild(CSTR_OFFENSE_MELE_DMG), offense.meleeDamage);

    // Attributes
    ::write(node.addChild(CSTR_ATTR_STR), attributes.str);
    ::write(node.addChild(CSTR_ATTR_DEX), attributes.dex);
    ::write(node.addChild(CSTR_ATTR_MANA), attributes.currentMana);
    ::write(node.addChild(CSTR_ATTR_MANA_MAX), attributes.maxMana);
    ::write(node.addChild(CSTR_ATTR_LIFE), attributes.currentLife);
    ::write(node.addChild(CSTR_ATTR_LIFE_MAX), attributes.maxLife);

    // Protection
    ::write(node.addChild(CSTR_PROT_WEAPON), protection.weapon);
    ::write(node.addChild(CSTR_PROT_ARROW), protection.arrow);
    ::write(node.addChild(CSTR_PROT_FIRE), protection.fire);
    ::write(node.addChild(CSTR_PROT_MAGIC), protection.magic);

    // Fighting Skills
    ::write(node.addChild(CSTR_FIGHT_1H), fightingSkills.oneHand);
    ::write(node.addChild(CSTR_FIGHT_2H), fightingSkills.twoHand);
    ::write(node.addChild(CSTR_FIGHT_BOW), fightingSkills.bow);
    ::write(node.addChild(CSTR_FIGHT_CBOW), fightingSkills.crossbow);
    ::write(node.addChild(CSTR_FIGHT_MAG), fightingSkills.magic);

    // Thieving Skills
    ::write(node.addChild(CSTR_THIEF_LOCKS), thievingSkills.openLocks);
    ::write(node.addChild(CSTR_THIEF_PICKPOCKET), thievingSkills.pickpocketing);
    ::write(node.addChild(CSTR_THIEF_SNEAK), thievingSkills.sneaking);

    // Special
    ::write(node.addChild(CSTR_SPEC_ACROBATICS), specialSkills.acrobatics);
}
}  // namespace Components
}  // namespace GameEngine
