#include "EquipmentComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <optional>

#include "EquippableComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/Animation/PoseUpdater.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterControllerEvents.h"
#include "GameEngine/Components/Gameplay/Attack/MeleeAttackComponent.h"
#include "GameEngine/Components/Gameplay/Attack/WeaponComponent.h"
#include "GameEngine/Components/Gameplay/CharacterStats/CharacterStatsComponent.h"
#include "GameEngine/Components/Gameplay/CharacterStats/CharacterStatsViewerComponent.h"
#include "GameEngine/Components/Gameplay/CharacterStats/ModifiableStat.h"
#include "GameEngine/Components/Gameplay/Inventory/CombatStatsComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/ItemVisualComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/SlotType.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "TreeNode.h"
#include "Types.h"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_BASE_BODY_RENDERER_TAG[] = "baseBodyRendererComponentTag";
constexpr char CHEST_COMPONENT_TAG[]{"ChestEquippedItem"};
constexpr char CSTR_MAIN_HAND_ITEM[]{"MainHandItem"};
constexpr char CSTR_EQUIP_JOINT_NAME[]{"equipJointName"};
constexpr char CSTR_DISARM_JOINT_NAME[]{"disarmJointName"};

ModifiableStat* getStatByTarget(TargetStat target, CharacterStatsComponent* stats)
{
    // clang-format off
    switch (target)
    {
        case TargetStat::Strength:           return &stats->attributes.str;
        case TargetStat::Dexterity:          return &stats->attributes.dex;
        case TargetStat::MaxMana:            return &stats->attributes.maxMana;
        case TargetStat::MaxLife:            return &stats->attributes.maxLife;
        case TargetStat::ProjectionWeapon:   return &stats->protection.weapon;
        case TargetStat::ProjectionArrow:    return &stats->protection.arrow;
        case TargetStat::ProjectionFire:     return &stats->protection.fire;
        case TargetStat::ProjectionMagic:    return &stats->protection.magic;
        case TargetStat::MeleDamage:         return &stats->offense.meleeDamage;
        case TargetStat::MeleeAttackSpeed:   return &stats->offense.meleeAttackSpeed;
        case TargetStat::RangedDamage:       return &stats->offense.rangedDamage;
        case TargetStat::RangedAttackSpeed:  return &stats->offense.rangedAttackSpeed;
        case TargetStat::MagicDamage:        return &stats->offense.magicDamage;
        case TargetStat::CastSpeed:          return &stats->offense.castSpeed;
    }
    // clang-format on

    return nullptr;
};
}  // namespace

REGISTER_COMPONENT(EquipmentComponent)

EquipmentComponent::EquipmentComponent(ComponentContext& componentContext, GameObject& gameObject)
    : Component(componentContext, gameObject)
{
}
EquipmentComponent::~EquipmentComponent()
{
}
void EquipmentComponent::CleanUp()
{
}
void EquipmentComponent::Reload()
{
}
void EquipmentComponent::ReqisterFunctions()
{
}
void EquipmentComponent::read(const TreeNode& input)
{
    ::Read(input.getChild(CSTR_BASE_BODY_RENDERER_TAG), baseBodyRendererComponentTag);
    ::Read(input.getChild(CSTR_EQUIP_JOINT_NAME), equipJointName);
    ::Read(input.getChild(CSTR_DISARM_JOINT_NAME), disarmJointName);
}
void EquipmentComponent::write(TreeNode& node) const
{
    ::write(node.addChild(CSTR_BASE_BODY_RENDERER_TAG), baseBodyRendererComponentTag);
    ::write(node.addChild(CSTR_EQUIP_JOINT_NAME), equipJointName);
    ::write(node.addChild(CSTR_DISARM_JOINT_NAME), disarmJointName);
}
bool EquipmentComponent::equip(GameObject& item)
{
    auto itemEquippableComponent = item.GetComponent<EquippableComponent>();
    if (not itemEquippableComponent)
    {
        LOG_WARN << "Try equip iteam without component";
        return false;
    }

    switch (itemEquippableComponent->slot)
    {
        case SlotType::MainHand:
            equipOneHand(item);
            break;
        case SlotType::Chest:
            equipChest(item);
            break;
        default:
            LOG_DEBUG << "Unsupported slot type";
    }

    equippedItems[itemEquippableComponent->slot] = &item;
    applyItemModifiers(item);

    return true;
}
bool EquipmentComponent::canEquip(GameObject&) const
{
    return true;
}
bool EquipmentComponent::isSlotFree(SlotType slot) const
{
    auto iter = equippedItems.find(slot);
    return iter == equippedItems.end() or iter->second == nullptr;
}
std::optional<IdType> EquipmentComponent::unequip(SlotType slot)
{
    LOG_DEBUG << magic_enum::enum_name(slot);

    if (isSlotFree(slot))
    {
        return std::nullopt;
    }

    if (slot == SlotType::MainHand)
    {
        unequipWeapon();
    }
    else if (slot == SlotType::Chest)
    {
        unequipChest();
        reloadAnimator();
    }

    auto iter = equippedItems.find(slot);
    if (iter != equippedItems.end() and iter->second)
    {
        auto itemId = iter->second->GetId();
        removeItemModifiers(itemId);
        iter->second = nullptr;
        return itemId;
    }
    return std::nullopt;
}
void EquipmentComponent::unequipChest()
{
    if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(CHEST_COMPONENT_TAG))
    {
        thisObject_.RemoveComponent(*renderComponent);
    }

    activeDefaultBody(true);
}
void EquipmentComponent::reloadAnimator()
{
    if (auto animator = thisObject_.GetComponent<Animator>())
    {
        animator->Reload();

        if (auto go = thisObject_.GetChild(CSTR_MAIN_HAND_ITEM))
        {
            if (auto updater = go->GetComponent<PoseUpdater>())
            {
                updater->Reload();
            }
        }
    }
}
void EquipmentComponent::equipChest(const GameObject& item)
{
    auto itemVisualComponent = item.GetComponent<ItemVisualComponent>();
    if (not itemVisualComponent)
    {
        LOG_WARN << "ItemVisualComponent not found";
        return;
    }

    if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(CHEST_COMPONENT_TAG))
    {
        if (itemVisualComponent->modelPath == renderComponent->fileName_LOD1)
        {
            LOG_DEBUG << "Model already equipped";
            return;
        }
        thisObject_.RemoveComponent(*renderComponent);
    }

    if (not baseBodyRendererComponentTag.empty())
    {
        activeDefaultBody(false);
    }

    if (const auto& node = itemVisualComponent->getRendererComponentNode())
    {
        if (auto newRendererComponent = thisObject_.AddComponent(*node))
        {
            newRendererComponent->SetTag(CHEST_COMPONENT_TAG);
            reloadAnimator();
        }
    }
}
void EquipmentComponent::equipOneHand(const GameObject& item)
{
    auto itemVisualComponent = item.GetComponent<ItemVisualComponent>();
    if (not itemVisualComponent)
    {
        LOG_WARN << "ItemVisualComponent not found";
        return;
    }

    auto newGameObject = componentContext_.scene_.CreateGameObject(CSTR_MAIN_HAND_ITEM);
    if (const auto& node = itemVisualComponent->getRendererComponentNode())
    {
        newGameObject->AddComponent(*node);
    }

    auto& pu            = newGameObject->AddComponent<PoseUpdater>();
    pu.disarmJointName_ = disarmJointName;
    pu.equipJointName_  = equipJointName;
    pu.Reload();

    if (not newGameObject->AddClonedComponent(item.GetComponent<WeaponComponent>()))
    {
        newGameObject->AddComponent<WeaponComponent>().socketOffsets.push_back(vec3(0));
    }

    if (not newGameObject->AddClonedComponent(item.GetComponent<CombatStatsComponent>()))
    {
        newGameObject->AddComponent<CombatStatsComponent>();
    }

    componentContext_.scene_.AddGameObject(std::move(newGameObject), &thisObject_);
}
void EquipmentComponent::activeDefaultBody(bool isActive)
{
    if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(baseBodyRendererComponentTag))
    {
        renderComponent->SetActive(isActive);
    }
}
void EquipmentComponent::unequipWeapon()
{
    if (auto go = thisObject_.GetChild(CSTR_MAIN_HAND_ITEM))
    {
        LOG_DEBUG << "mainHand object found, remove";
        componentContext_.scene_.RemoveGameObject(*go);

        if (auto cc = thisObject_.GetComponent<CharacterController>())
        {
            cc->pushEventToQueue(WeaponStateEvent{});
        }
    }

    if (auto mac = thisObject_.GetComponent<MeleeAttackComponent>())
    {
        mac->clearWeapon();
    }
}
void EquipmentComponent::applyItemModifiers(GameObject& item)
{
    auto characterStats = thisObject_.GetComponent<CharacterStatsComponent>();
    if (not characterStats)
        return;

    auto itemStats = item.GetComponent<CombatStatsComponent>();
    if (not itemStats)
        return;

    auto itemId = item.GetId();
    for (const auto& mod : itemStats->modifiers)
    {
        if (auto* stat = getStatByTarget(mod.target, characterStats))
        {
            LOG_DEBUG << "applyItemModifiers " << magic_enum::enum_name(mod.target) << " value " << mod.value;
            stat->addModifier(StatSource::Equipment, itemId, mod.type, mod.value);
        }
    }

    if (auto viewer = thisObject_.GetComponent<CharacterStatsViewerComponent>())
    {
        viewer->updateGuiStats();
    }
}
void EquipmentComponent::removeItemModifiers(IdType itemId)
{
    auto characterStats = thisObject_.GetComponent<CharacterStatsComponent>();
    if (not characterStats)
        return;

    auto& charAttrs   = characterStats->attributes;
    auto& charProtect = characterStats->protection;
    auto& charOffense = characterStats->offense;

    // Attributes
    charAttrs.str.removeModifiersFromSource(itemId);
    charAttrs.dex.removeModifiersFromSource(itemId);
    charAttrs.maxLife.removeModifiersFromSource(itemId);
    charAttrs.maxMana.removeModifiersFromSource(itemId);

    // Protection
    charProtect.weapon.removeModifiersFromSource(itemId);
    charProtect.arrow.removeModifiersFromSource(itemId);
    charProtect.fire.removeModifiersFromSource(itemId);
    charProtect.magic.removeModifiersFromSource(itemId);

    // Offense
    charOffense.meleeDamage.removeModifiersFromSource(itemId);
    charOffense.meleeAttackSpeed.removeModifiersFromSource(itemId);
    charOffense.rangedDamage.removeModifiersFromSource(itemId);
    charOffense.rangedAttackSpeed.removeModifiersFromSource(itemId);
    charOffense.magicDamage.removeModifiersFromSource(itemId);
    charOffense.castSpeed.removeModifiersFromSource(itemId);

    if (auto viewer = thisObject_.GetComponent<CharacterStatsViewerComponent>())
    {
        viewer->updateGuiStats();
    }
}
}  // namespace Components
}  // namespace GameEngine
