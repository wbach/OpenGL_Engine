#pragma once
#include <Input/KeysSubscriptionsManager.h>
#include <TreeNode.h>

#include "GameEngine/Components/Component.h"
#include "GameEngine/Components/Gameplay/Inventory/CombatStatsComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/ItemIdentityComponent.h"
#include "GameEngine/Components/Gameplay/Inventory/ItemVisualComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Prefab.h"
#include "GameEngine/Renderers/GUI/Text/MultiLineText.h"
#include "GameEngine/Resources/File.h"
#include "Types.h"

namespace GameEngine
{
namespace GUI
{
class Layer;
class Button;
class Element;
class VerticalLayout;
class Window;
class Text;
class Sprite;
}  // namespace GUI

class GameState;
namespace Components
{
class ConsumableComponent;
class EquippableComponent;
class CombatStatsComponent;
class ItemIdentityComponent;
class ItemVisualComponent;
DECLARE_COMPONENT(InventoryComponent)
{
public:
    File guiFile;
    File itemsFile;

public:
    // clang-format off
    BEGIN_FIELDS()
        FIELD_FILE(guiFile)
        FIELD_FILE(itemsFile)
    END_FIELDS()
    // clang-format on

public:
    using Items = std::vector<std::unique_ptr<Prefab>>;
    InventoryComponent(ComponentContext&, GameObject&);

    InventoryComponent(const InventoryComponent&)                = delete;
    InventoryComponent& operator=(const InventoryComponent&)     = delete;
    InventoryComponent(InventoryComponent &&) noexcept           = default;
    InventoryComponent& operator=(InventoryComponent&&) noexcept = default;

    ~InventoryComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void addItem(std::unique_ptr<Prefab> &&);
    void addItem(std::unique_ptr<GameObject> &&);
    void addItem(const GameObject&);

    void useItem(GameObject&);

private:
    void initGui();
    void updateGui();
    void show();
    void hide();
    void readInventory();
    void writeInventory();
    void toneDownCategoryBtns();
    void applyConsumable(GameObject&, ConsumableComponent&);
    void handleEquipping(GameObject&, EquippableComponent&);

    EquippableComponent* getItem(IdType);
    std::vector<GameObject*> applyFilterToItems();

    Color getDefaultItemSpriteColor() const;
    vec2 calculateCursorPosition(const GUI::Element&);

    void updateItemDetailsWindow(const CombatStatsComponent&, const ItemIdentityComponent&, const ItemVisualComponent&);

private:
    Input::KeysSubscriptionsManager keySubManager;
    std::string currentCategory;

    struct UiSlot
    {
        GUI::Button* button;
        std::optional<IdType> itemId;
    };
    std::vector<UiSlot> uiSlots;
    std::vector<GUI::Button*> categoryButtons;

    GUI::Layer* layer{nullptr};
    GUI::Element* group{nullptr};

    GUI::Window* itemDetails{nullptr};
    GUI::Text* itemNameTxt{nullptr};
    GUI::MultiLineText* itemDescription{nullptr};
    GUI::Sprite* itemImage{nullptr};
    GUI::VerticalLayout* itemStats{nullptr};
    std::optional<TreeNode> itemStatNode;

    Items items;

    GUI::VerticalLayout* itemsLayout;

    std::optional<TreeNode> defaultItemSpriteNode;

public:
    void read(const TreeNode&) override;
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
