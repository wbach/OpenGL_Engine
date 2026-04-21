#pragma once
#include <TreeNode.h>

#include "GameEngine/Components/BaseComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Objects/Prefab.h"
#include "GameEngine/Resources/File.h"
#include "Types.h"

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
class ENGINE_API InventoryComponent : public BaseComponent
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
    InventoryComponent(ComponentContext&, GameObject&);
    ~InventoryComponent() override;

    void CleanUp() override;
    void ReqisterFunctions() override;
    void Reload() override;

    void addItem(std::unique_ptr<Prefab>&&);
    void addItem(std::unique_ptr<GameObject>&&);
    void addItem(const GameObject&);

private:
    void initGui();
    void updateGui();
    void show();
    void hide();
    void readInventory();
    void writeInventory();

private:
    std::string currentCategory = "Weapons";

    struct UiSlot
    {
        GUI::Button* button;
        std::optional<IdType> itemId;
    };
    std::vector<UiSlot> uiSlots;

    GUI::Window* mainWindow{nullptr};
    std::vector<std::unique_ptr<Prefab>> items;
    GUI::VerticalLayout* itemsLayout;

    TreeNode defaultItemSpriteNode;

public:
    static void registerReadFunctions();
    void write(TreeNode&) const override;
};
}  // namespace Components
}  // namespace GameEngine
