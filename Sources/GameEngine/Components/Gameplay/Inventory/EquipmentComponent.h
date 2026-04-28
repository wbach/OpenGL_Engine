#pragma once
#include <optional>
#include <string>
#include <unordered_map>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "SlotType.h"

namespace GameEngine
{
class GameState;
namespace Components
{
class ENGINE_API EquipmentComponent : public BaseComponent
{
public:
public:
    std::string baseBodyRendererComponentTag;
    // clang-format off
    BEGIN_FIELDS()
        FIELD_STRING(baseBodyRendererComponentTag)
    END_FIELDS()
    // clang-format on

public:
    EquipmentComponent(ComponentContext&, GameObject&);
    ~EquipmentComponent() override;
    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;
    void readFile();

    bool equip(GameObject&);
    bool canEquip(GameObject&) const;
    bool isSlotFree(SlotType) const;

    std::optional<IdType> unequip(SlotType);

private:
    void equipChest(const GameObject&);
    void equipOneHand(const GameObject&);
    void unequipWeapon();
    void unequipChest();
    void activeDefaultBody(bool);
    void reloadAnimator();

private:
    std::unordered_map<SlotType, GameObject*> equippedItems;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
