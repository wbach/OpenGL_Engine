#include "CombatStatsComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include "GameEngine/Components/ComponentsReadFunctions.h"

namespace GameEngine
{
namespace
{
const char CSTR_DAMAGE[]        = "damage";
const char CSTR_ATTACK_SPEED[]  = "attackSpeed";
const char CSTR_RANGE[]         = "range";
const char CSTR_PHYS_DEF[]      = "physicalDefense";
const char CSTR_MAG_DEF[]       = "magicDefense";
const char CSTR_FIRE_RES[]      = "fireResistance";
const char CSTR_HP_BONUS[]      = "hpBonus";
const char CSTR_MANA_BONUS[]    = "manaBonus";
const char CSTR_STAMINA_BONUS[] = "staminaBonus";
}  // namespace

namespace Components
{
REGISTER_COMPONENT(CombatStatsComponent)

CombatStatsComponent::CombatStatsComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}

CombatStatsComponent::~CombatStatsComponent()
{
}

void CombatStatsComponent::CleanUp()
{
}

void CombatStatsComponent::Reload()
{
}

void CombatStatsComponent::ReqisterFunctions()
{
}

void CombatStatsComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_DAMAGE), damage);
    ::Read(input.getChild(CSTR_ATTACK_SPEED), attackSpeed);
    ::Read(input.getChild(CSTR_RANGE), range);
    ::Read(input.getChild(CSTR_PHYS_DEF), physicalDefense);
    ::Read(input.getChild(CSTR_MAG_DEF), magicDefense);
    ::Read(input.getChild(CSTR_FIRE_RES), fireResistance);
    ::Read(input.getChild(CSTR_HP_BONUS), hpBonus);
    ::Read(input.getChild(CSTR_MANA_BONUS), manaBonus);
    ::Read(input.getChild(CSTR_STAMINA_BONUS), staminaBonus);
}

void CombatStatsComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_DAMAGE), damage);
    ::write(node.addChild(CSTR_ATTACK_SPEED), attackSpeed);
    ::write(node.addChild(CSTR_RANGE), range);
    ::write(node.addChild(CSTR_PHYS_DEF), physicalDefense);
    ::write(node.addChild(CSTR_MAG_DEF), magicDefense);
    ::write(node.addChild(CSTR_FIRE_RES), fireResistance);
    ::write(node.addChild(CSTR_HP_BONUS), hpBonus);
    ::write(node.addChild(CSTR_MANA_BONUS), manaBonus);
    ::write(node.addChild(CSTR_STAMINA_BONUS), staminaBonus);
}

}  // namespace Components
}  // namespace GameEngine
