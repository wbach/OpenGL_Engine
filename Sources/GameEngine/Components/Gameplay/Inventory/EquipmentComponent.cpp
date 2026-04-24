#include "EquipmentComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <optional>

#include "EquippableComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Animation/PoseUpdater.h"
#include "GameEngine/Components/Animation/JointPoseUpdater.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Gameplay/Inventory/ItemVisualComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/SlotType.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
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
}  // namespace
EquipmentComponent::EquipmentComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<EquipmentComponent>(), componentContext, gameObject)
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
void EquipmentComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<EquipmentComponent>(componentContext, gameObject);
        component->read(input);
        ::Read(input.getChild(CSTR_BASE_BODY_RENDERER_TAG), component->baseBodyRendererComponentTag);
        return component;
    };

    regsiterComponentReadFunction(GetComponentType<EquipmentComponent>(), func);
}
void EquipmentComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);
    node.addChild(CSTR_BASE_BODY_RENDERER_TAG, baseBodyRendererComponentTag);
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

    if (slot == SlotType::MainHand)
    {
        if (auto go = thisObject_.GetChild(CSTR_MAIN_HAND_ITEM))
        {
            LOG_DEBUG << "mainHand object found, remove";
            componentContext_.scene_.RemoveGameObject(*go);
        }
    }
    else if (slot == SlotType::Chest)
    {
        unequipChest();
        reloadAnimator();
    }

    auto iter = equippedItems.find(slot);
    if (iter != equippedItems.end() and iter->second)
    {
        auto result  = iter->second->GetId();
        iter->second = nullptr;
        return result;
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
    pu.disarmJointName_ = "Slot_MeleeBackLeft";
    pu.equipJointName_  = "Slot_RightHand_Weapon";

    pu.Reload();

    componentContext_.scene_.AddGameObject(std::move(newGameObject), &thisObject_);
}
void EquipmentComponent::activeDefaultBody(bool isActive)
{
    if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(baseBodyRendererComponentTag))
    {
        renderComponent->SetActive(isActive);
    }
}
}  // namespace Components
}  // namespace GameEngine
