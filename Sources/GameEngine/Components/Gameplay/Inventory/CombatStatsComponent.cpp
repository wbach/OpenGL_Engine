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
CombatStatsComponent::CombatStatsComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<CombatStatsComponent>(), componentContext, gameObject)
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

void CombatStatsComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<CombatStatsComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(CSTR_DAMAGE), component->damage);
        ::Read(input.getChild(CSTR_ATTACK_SPEED), component->attackSpeed);
        ::Read(input.getChild(CSTR_RANGE), component->range);
        ::Read(input.getChild(CSTR_PHYS_DEF), component->physicalDefense);
        ::Read(input.getChild(CSTR_MAG_DEF), component->magicDefense);
        ::Read(input.getChild(CSTR_FIRE_RES), component->fireResistance);
        ::Read(input.getChild(CSTR_HP_BONUS), component->hpBonus);
        ::Read(input.getChild(CSTR_MANA_BONUS), component->manaBonus);
        ::Read(input.getChild(CSTR_STAMINA_BONUS), component->staminaBonus);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<CombatStatsComponent>(), func);
}

void CombatStatsComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

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
