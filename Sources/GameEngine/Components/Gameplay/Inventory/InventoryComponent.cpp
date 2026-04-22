#include "InventoryComponent.h"

#include <Input/InputManager.h>
#include <Logger/Log.h>
#include <Utils/TreeNodeReadFunctions.h>
#include <Utils/TreeNodeWriteFunctions.h>

#include <Utils/FileSystem/FileSystemUtils.hpp>
#include <optional>
#include <utility>

#include "CombatStatsComponent.h"
#include "ConsumableComponent.h"
#include "EquippableComponent.h"
#include "GameEngine/Components/ComponentContext.h"
#include "GameEngine/Components/ComponentsReadFunctions.h"
#include "GameEngine/Objects/GameObject.h"
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
#include "GameEngine/Scene/SceneReader.h"
#include "GameEngine/Scene/SceneWriter.h"
#include "ItemIdentityComponent.h"
#include "ItemVisualComponent.h"
#include "Json/JsonReader.h"
#include "Json/JsonWriter.h"
#include "TreeNode.h"
namespace GameEngine
{
namespace Components
{
namespace
{
constexpr char GUI_FILE[]{"guiLayoutFile"};
constexpr char INVENTORY_FILE[]{"inventoryFile"};

std::string getCategoryForItem(GameObject& item)
{
    if (item.GetComponent<CombatStatsComponent>() and item.GetComponent<EquippableComponent>())
    {
        auto slot = item.GetComponent<EquippableComponent>()->slot;
        if (slot == SlotType::MainHand or slot == SlotType::BothHands)
            return "Weapons";
        return "Armors";
    }
    if (item.HasComponent<ConsumableComponent>())
        return "Potions";
    return "Other";
}
}  // namespace

InventoryComponent::InventoryComponent(ComponentContext& componentContext, GameObject& gameObject)
    : BaseComponent(GetComponentType<InventoryComponent>(), componentContext, gameObject)
{
}
InventoryComponent::~InventoryComponent()
{
}
void InventoryComponent::CleanUp()
{
}
void InventoryComponent::Reload()
{
}
void InventoryComponent::ReqisterFunctions()
{
    RegisterFunction(FunctionType::Awake,
                     [this]()
                     {
                         initGui();
                         readInventory();
                     });

    RegisterFunction(FunctionType::OnStart,
                     [this]()
                     {
                         componentContext_.inputManager_.SubscribeOnKeyDown(Input::GameAction::INVENTORY_VIEW,
                                                                            [this]()
                                                                            {
                                                                                if (not mainWindow)
                                                                                    return;

                                                                                if (mainWindow->isActive())
                                                                                {
                                                                                    hide();
                                                                                }
                                                                                else
                                                                                {
                                                                                    show();
                                                                                }
                                                                            });
                     });
}
void InventoryComponent::registerReadFunctions()
{
    auto func = [](ComponentContext& componentContext, const TreeNode& input, GameObject& gameObject)
    {
        auto component = std::make_unique<InventoryComponent>(componentContext, gameObject);
        component->read(input);

        ::Read(input.getChild(GUI_FILE), component->guiFile);
        ::Read(input.getChild(INVENTORY_FILE), component->itemsFile);

        return component;
    };

    regsiterComponentReadFunction(GetComponentType<InventoryComponent>(), func);
}
void InventoryComponent::write(TreeNode& node) const
{
    node.attributes_.insert({CSTR_TYPE, GetTypeName()});
    BaseComponent::write(node);

    ::write(node.addChild(GUI_FILE), guiFile);
    ::write(node.addChild(INVENTORY_FILE), itemsFile);
}
void InventoryComponent::initGui()
{
    if (mainWindow)
    {
        LOG_DEBUG << "Already initialized";
        return;
    }

    const std::string layerName{"InventoryViewer"};
    GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);

    if (reader.read(guiFile, layerName))
    {
        auto layer = componentContext_.guiManager_.getLayer(layerName);

        mainWindow  = GUI::getTypedElement<GUI::Window>(layer, "MainWindow");
        itemsLayout = GUI::getTypedElement<GUI::VerticalLayout>(layer, "ItemsLayout");

        std::vector<std::string> catNames = {"Weapons", "Armors", "Potions", "Magic", "Other"};

        int i = 0;
        for (const auto& name : catNames)
        {
            if (auto btn = GUI::getTypedElement<GUI::Button>(layer, name))
            {
                btn->setOnClick(
                    [this, name]()
                    {
                        currentCategory = name;
                        updateGui();
                    });

                if (i == 0)
                {
                    if (auto btn = GUI::getTypedElement<GUI::Button>(layer, name))
                        btn->setAsActive();
                }
            }
            ++i;
        }

        uiSlots.clear();

        if (itemsLayout)
        {
            for (auto& row : itemsLayout->getChildren())
            {
                for (auto& slot : row->getChildren())
                {
                    if (auto button = dynamic_cast<GUI::Button*>(slot.get()))
                    {
                        uiSlots.push_back(UiSlot{.button = button, .itemId = std::nullopt});
                    }
                }
            }

            if (not uiSlots.empty())
            {
                if (auto spirte = uiSlots.front().button->getBackgroundSprite())
                {
                    defaultItemSpriteNode.emplace();
                    GUI::ElementWriter::write(*defaultItemSpriteNode, *spirte);
                }
            }
        }

        if (auto exitButton = GUI::getTypedElement<GUI::Button>(layer, "Exit"))
        {
            exitButton->setOnClick([this]() { hide(); });
        }

        if (mainWindow)
        {
            mainWindow->activate(false);
        }

        updateGui();
    }
    else
    {
        LOG_WARN << "Inventory viewer init gui error";
    }
}

void InventoryComponent::updateGui()
{
    if (not mainWindow or not mainWindow->isActive() or uiSlots.empty())
        return;

    std::vector<GameObject*> filteredItems;
    for (auto& item : items)
    {
        auto object = item->getObject();
        if (not object)
        {
            continue;
        }

        const auto& category = getCategoryForItem(*object);
        LOG_DEBUG << "Check item=" << object->GetName() << ", category=" << category << ", currentCategory = " << currentCategory;
        if (category == currentCategory)
        {
            LOG_DEBUG << "item is visible " << object->GetName();
            filteredItems.push_back(object);
        }
    }

    for (size_t i = 0; i < uiSlots.size(); ++i)
    {
        auto& slot = uiSlots[i];

        if (i < filteredItems.size())
        {
            auto* item    = filteredItems[i];
            auto identity = item->GetComponent<ItemIdentityComponent>();

            if (identity)
            {
                if (slot.itemId == identity->getId())
                {
                    continue;
                }

                slot.itemId = identity->getId();

                if (auto visualComponent = item->GetComponent<ItemVisualComponent>())
                {
                    LOG_DEBUG << "Create item icon : " << visualComponent->iconPath;
                    if (auto sprite = componentContext_.guiElementFactory_.createSprite(visualComponent->iconPath))
                    {
                        slot.button->setBackground(std::move(sprite));
                    }
                }
                else
                {
                    LOG_WARN << "Item dosent have a ItemVisualComponent";
                }

                slot.button->setOnClick(
                    [item, identity]()
                    {
                        LOG_DEBUG << "Item gameObject name: " << item->GetName() << ", Item name: " << identity->itemName << " "
                                  << identity->description;
                    });
            }
            else
            {
                LOG_WARN << "Item dosent have a ItemIdentityComponent. Gamobject name: " << item->GetName();
            }
        }
        else
        {
            if (not slot.itemId.has_value())
            {
                continue;
            }

            LOG_DEBUG << "defaultItemSpriteNode : " << defaultItemSpriteNode;
            if (defaultItemSpriteNode)
            {
                GUI::ElementReader reader(componentContext_.guiManager_, componentContext_.guiElementFactory_);
                auto sprite = reader.readSprite(*defaultItemSpriteNode);
                slot.button->setBackground(std::move(sprite));
            }
            slot.button->setOnClick(nullptr);
            slot.itemId.reset();
        }
    }
}
void InventoryComponent::show()
{
    mainWindow->activate(true);

    componentContext_.inputManager_.SetReleativeMouseMode(false);
    componentContext_.inputManager_.ShowCursor(true);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Lock();
        updateGui();
    }
}
void InventoryComponent::hide()
{
    mainWindow->activate(false);

    componentContext_.inputManager_.SetReleativeMouseMode(true);
    componentContext_.inputManager_.ShowCursor(false);

    if (auto mainCamera = componentContext_.scene_.GetCameraManager().GetMainCamera())
    {
        mainCamera->Unlock();
    }
}
void InventoryComponent::addItem(std::unique_ptr<Prefab>&& item)
{
    if (item)
    {
        LOG_DEBUG << "Add prefab item";
        items.push_back(std::move(item));
        updateGui();
    }
}

void InventoryComponent::addItem(std::unique_ptr<GameObject>&& item)
{
    if (item)
    {
        addItem(*item);
    }
}

void InventoryComponent::addItem(const GameObject& item)
{
    auto prefabRootNode = GameEngine::createPrefab(item);
    if (auto prefabObject = GameEngine::SceneReader(componentContext_.scene_).createPrefabGameObject(prefabRootNode))
    {
        items.push_back(std::move(prefabObject));
    }
    updateGui();
}

void InventoryComponent::readInventory()
{
    if (not itemsFile.exist())
    {
        LOG_DEBUG << "File not exist: " << itemsFile;
        return;
    }

    Utils::JsonReader reader;
    reader.Read(itemsFile.GetAbsolutePath());

    if (auto itemsNodes = reader.Get("Items"))
    {
        LOG_DEBUG << "itemsNodes " << *itemsNodes;

        for (auto& child : itemsNodes->getChildren())
        {
            auto name = child->getChild("name");
            auto file = child->getChild("file");

            if (name and file)
            {
                LOG_DEBUG << "Read item: " << name->value_ << ", file = " << file->value_;

                if (auto gameObject = GameEngine::SceneReader(componentContext_.scene_).readPrefab(file->value_, name->value_))
                {
                    addItem(std::move(gameObject));
                }
            }
        }
    }

    updateGui();
}

void InventoryComponent::writeInventory()
{
    TreeNode root;
    auto& itemsNode = root.addChild("Items");

    for (auto& item : items)
    {
        auto& itemEntry = itemsNode.addChild("");
        itemEntry.addChild("name", item->GetName());
        itemEntry.addChild("file", item->getFile().GetDataRelativePath());
    }
    if (itemsFile.empty())
    {
        itemsFile = EngineLocalConf.files.getGeneratedDirPath() / ("Inventory_" + Utils::CreateUniqueFilename() + ".json");
    }
    Utils::Json::Write(itemsFile.GetAbsolutePath(), root);
}
}  // namespace Components
}  // namespace GameEngine
