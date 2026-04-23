#include "EquipmentComponent.h"

#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <optional>

#include "EquippableComponent.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Components/Controllers/CharacterController/CharacterController.h"
#include "GameEngine/Components/Gameplay/Inventory/ItemVisualComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/SlotType.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char CSTR_BASE_BODY_RENDERER_TAG[] = "baseBodyRendererComponentTag";
const std::string CHEST_COMPONENT_TAG{"ChestEquippedItem"};
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

    auto itemVisualComponent = item.GetComponent<ItemVisualComponent>();
    auto iter                = equippedItems.find(itemEquippableComponent->slot);
    switch (itemEquippableComponent->slot)
    {
        case SlotType::Chest:
        {
            if (iter != equippedItems.end() and iter->second)
            {
                // Takeof current model
                if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(CHEST_COMPONENT_TAG))
                {
                    if (itemVisualComponent->modelPath == renderComponent->fileName_LOD1)
                    {
                        LOG_DEBUG << "Model already equipped";
                        break;
                    }

                    thisObject_.RemoveComponent(*renderComponent);
                }

                if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(baseBodyRendererComponentTag))
                {
                    renderComponent->SetActive(true);
                }

                iter->second = nullptr;
            }
            else if (not baseBodyRendererComponentTag.empty())
            {
                if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(baseBodyRendererComponentTag))
                {
                    renderComponent->SetActive(false);
                }
            }

            if (itemVisualComponent)
            {
                LOG_DEBUG << "Add rendererComponent";
                auto& newRendererComponent         = thisObject_.AddComponent<RendererComponent>();
                newRendererComponent.fileName_LOD1 = itemVisualComponent->modelPath;
                newRendererComponent.tag           = CHEST_COMPONENT_TAG;
                newRendererComponent.Reload();

                if (auto animator = thisObject_.GetComponent<Animator>())
                {
                    animator->Reload();
                }
            }
        }
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
    LOG_DEBUG << "";

    if (slot == SlotType::Chest)
    {
        if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(CHEST_COMPONENT_TAG))
        {
            thisObject_.RemoveComponent(*renderComponent);
        }
        if (auto renderComponent = thisObject_.GetComponent<RendererComponent>(baseBodyRendererComponentTag))
        {
            renderComponent->SetActive(true);
        }
        if (auto animator = thisObject_.GetComponent<Animator>())
        {
            animator->Reload();
        }
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
}  // namespace Components
}  // namespace GameEngine
