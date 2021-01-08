#include "GuiEditScene.h"
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <GameEngine/Renderers/GUI/EditText/GuiEditText.h>
#include <GameEngine/Renderers/GUI/GuiElementFactory.h>
#include <GameEngine/Renderers/GUI/Layout/HorizontalLayout.h>
#include <GameEngine/Renderers/GUI/Layout/Layout.h>
#include <GameEngine/Renderers/GUI/Layout/VerticalLayout.h>
#include <GameEngine/Renderers/GUI/Text/GuiTextElement.h>
#include <GameEngine/Renderers/GUI/Texutre/GuiTextureElement.h>
#include <GameEngine/Renderers/GUI/TreeView/TreeView.h>
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Input/InputManager.h>
#include <algorithm>
#include "FileExplorer.h"
#include "FileSystem/FileSystemUtils.hpp"
#include "Editor/Context.h"

using namespace GameEngine;

namespace Editor
{
namespace
{
const std::string NEW_LAYER_NAME = "NewLayer";
const std::string TEMP_FILE_NAME = "fileExplorer.tmp";
}  // namespace
GuiEditScene::GuiEditScene(Context& context)
    : GameEngine::Scene("GuiEditScene")
    , context_(context)
    , mousePosition_(0)
    , notCleanLayers_{"DefaultLayer"}
    , multiSelect_{false}
    , objectTree_{nullptr}
{
}
GuiEditScene::~GuiEditScene()
{
    SaveLastOpenedLocation();
}
int GuiEditScene::Initialize()
{
    resourceManager_->GetGraphicsApi().SetBackgroundColor(context_.backgorundColor);

    ReadLastOpenedLocation();
    AddStartupActions();
    AddMenuButtonAction();
    ReadMenu();
    KeyOperations();
    return 0;
}

void GuiEditScene::PostInitialize()
{
}
int GuiEditScene::Update(float)
{
    if (not multiSelect_)
    {
        auto position  = inputManager_->GetMousePosition();
        auto mouseVec  = position - mousePosition_;
        mousePosition_ = position;

        for (auto& element : guiElementsChoose_)
        {
            const auto& pos = element->GetLocalPosition();
            element->SetLocalPostion(pos + mouseVec);
        }
    }
    return 0;
}

void GuiEditScene::KeyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::LCTRL, [this]() { multiSelect_ = true; });
    inputManager_->SubscribeOnKeyUp(KeyCodes::LCTRL, [this]() { multiSelect_ = false; });
    inputManager_->SubscribeOnKeyUp(KeyCodes::LMOUSE, [this]() {
        if (not multiSelect_)
        {
            guiElementsChoose_.clear();
        }
    });

    EnableAlginToFirstX();
    EnableSetTheSameYDistance();
}

void GuiEditScene::AddStartupActions()
{
    guiManager_->RegisterAction("FillObjectTree()", [this](auto& element) {
        DEBUG_LOG("FillObjectTree called");

        if (element.GetType() != GuiElementTypes::TreeView)
        {
            ERROR_LOG("Type mismatch, treeView dedicated function");
            return;
        }

        objectTree_ = static_cast<TreeView*>(&element);

        //            auto test1    = objectTree_->Add("Test1");
        //            auto test1_1  = objectTree_->Add("Test1_1", test1);
        //            auto test2_1  = objectTree_->Add("Test2_1", test1_1);
        //            auto test3_1  = objectTree_->Add("Test3_1", test2_1);
        //            objectTree_->Add("Test4_1", test3_1);
        //            objectTree_->Add("Test2");
    });

    guiManager_->RegisterAction("FillGuiElementTypes()", [&](auto& element) {
        if (element.GetType() == GuiElementTypes::VerticalLayout or
            element.GetType() == GuiElementTypes::HorizontalLayout)
        {
            auto layout = static_cast<Layout*>(&element);

            if (layout)
            {
                if (currentLayer_.empty())
                {
                    if (not guiManager_->GetLayer(NEW_LAYER_NAME))
                        guiManager_->AddLayer(NEW_LAYER_NAME);

                    currentLayer_ = NEW_LAYER_NAME;
                }

                for (auto type : GUI_ELEMENT_TYPES)
                {
                    auto button = guiElementFactory_->CreateGuiButton(std::to_string(type), [this, type](auto&) {
                        ShowCreateWindow(type);

                        auto window = guiManager_->GetElement("chooseComponentTypeWindow");
                        if (window)
                        {
                            guiManager_->AddTask([this, window]() { guiManager_->Remove(*window); });
                        }
                    });
                    button->SetIsInternal(true);
                    layout->AddChild(std::move(button));
                }
            }
        }
    });
}

void GuiEditScene::FillGuiElementInTreeObject(const std::unique_ptr<GuiElement>& element, std::optional<uint32> parent)
{
    auto printedText = std::to_string(element->GetType());

    if (not element->IsShow())
    {
        printedText += " (Hide)";
    }

    if (not element->IsActive())
    {
        printedText += " (Disabled)";
    }

    if (element->GetType() == GuiElementTypes::Text)
    {
        auto guiText = static_cast<GuiTextElement*>(element.get());
        printedText += " (" + guiText->GetText() + ")";
    }
    else if (element->GetType() == GuiElementTypes::Texture)
    {
        auto guiTexture = static_cast<GuiTextureElement*>(element.get());
        if (guiTexture and guiTexture->GetTexture() and guiTexture->GetTexture()->GetFile())
        printedText += " (" + guiTexture->GetTexture()->GetFile()->GetBaseName() + ")";
    }

    auto id = objectTree_->Add(printedText, parent);

    for (auto& child : element->GetChildren())
    {
        FillGuiElementInTreeObject(child, id);
    }
}

void GuiEditScene::AddMenuButtonAction()
{
    guiManager_->RegisterAction("ReadFile()", [&](auto&) {
        fileExplorer_  = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        auto dirToOpen = Utils::GetCurrentDir();
        if (not lastOpenedLocation_.empty())
        {
            dirToOpen = lastOpenedLocation_;
        }

        fileExplorer_->Start(dirToOpen, [&](const std::string& str) {
            guiManager_->RemoveLayersExpect(notCleanLayers_);
            processingFilename_ = str;
            currentLayer_       = str;
            lastOpenedLocation_ = Utils::GetParent(str);

            auto result = guiElementFactory_->ReadGuiFile(str);

            if (objectTree_ and result)
            {
                auto currentLayer = guiManager_->GetLayer(currentLayer_);
                for (auto& element : currentLayer->GetElements())
                {
                    FillGuiElementInTreeObject(element);
                }
            }

            return result;
        });
    });

    guiManager_->RegisterAction("SaveToFile()", [&](auto&) {
        fileExplorer_  = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        auto dirToOpen = Utils::GetCurrentDir();
        if (not lastOpenedLocation_.empty())
        {
            dirToOpen = lastOpenedLocation_;
        }

        fileExplorer_->Start(dirToOpen,
                             [&](const std::string& str) { return guiManager_->SaveToFile(str, currentLayer_); });
    });

    guiManager_->RegisterAction("QuickSave()",
                                [&](auto&) { guiManager_->SaveToFile(processingFilename_, currentLayer_); });

    guiManager_->RegisterAction("Reload()", [&](auto&) {
        guiManager_->RemoveLayersExpect(notCleanLayers_);
        guiElementFactory_->ReadGuiFile(processingFilename_);
    });

    guiManager_->RegisterAction("Clear()", [&](auto&) {
        currentLayer_.clear();
        guiManager_->RemoveLayersExpect(notCleanLayers_);
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::LMOUSE, [&]() {
        auto mousePosition = inputManager_->GetMousePosition();

        auto layer = guiManager_->GetLayer(currentLayer_);

        if (layer)
        {
            GuiElement* result{nullptr};
            for (const auto& layerElement : layer->GetElements())
            {
                if (not layerElement->IsShow() or layerElement->IsInternal())
                {
                    continue;
                }

                auto element = layerElement->GetCollisonElement(mousePosition);
                if (element)
                {
                    if (result and result->GetZValue() < element->GetZValue())
                    {
                        continue;
                    }
                    result = element;
                }
            }
            if (result)
            {
                auto existElement = std::find_if(guiElementsChoose_.begin(), guiElementsChoose_.end(),
                                                 [result](auto el) { return el->GetId() == result->GetId(); });

                if (existElement == guiElementsChoose_.end())
                {
                    guiElementsChoose_.push_back(result);
                }
            }
        }
    });

    guiManager_->RegisterAction("AddElement()", [&](auto&) {
        auto fullName = EngineConf_GetFullDataPath("Scenes/Editor/chooseComponentType.xml");
        guiElementFactory_->ReadGuiFile(fullName);
    });

    guiManager_->RegisterAction("DeleteElement()", [&](auto&) {
        for (auto& element : guiElementsChoose_)
            guiManager_->Remove(*element);

        guiElementsChoose_.clear();
    });
}

void GuiEditScene::ReadGuiFile(const std::string& file)
{
    auto fullName = EngineConf_GetFullDataPath(file);
    guiElementFactory_->ReadGuiFile(fullName);
    notCleanLayers_.push_back(fullName);
    guiManager_->GetLayer(fullName)->SetZPosition(-1.f);
}

void GuiEditScene::ReadMenu()
{
    ReadGuiFile("Scenes/Editor/CommonMenu.xml");
    ReadGuiFile("Scenes/Editor/ObjectTree.xml");
}

void GuiEditScene::EnableAlginToFirstX()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::X, [&]() {
        if (not guiElementsChoose_.empty())
        {
            auto x = guiElementsChoose_[0]->GetLocalPosition().x;

            for (auto& element : guiElementsChoose_)
            {
                auto pos = element->GetLocalPosition();
                pos.x    = x;
                element->SetLocalPostion(pos);
            }
        }
    });
}

void GuiEditScene::EnableSetTheSameYDistance()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::Y, [&]() {
        if (guiElementsChoose_.size() > 2)
        {
            auto distance = guiElementsChoose_[1]->GetLocalPosition().y - guiElementsChoose_[0]->GetLocalPosition().y;

            for (size_t i = 2; i < guiElementsChoose_.size(); ++i)
            {
                auto pos = guiElementsChoose_[i]->GetLocalPosition();
                pos.y    = guiElementsChoose_[i - 1]->GetLocalPosition().y + distance;
                guiElementsChoose_[i]->SetLocalPostion(pos);
            }
        }
    });
}

void GuiEditScene::ShowCreateWindow(GuiElementTypes type)
{
    switch (type)
    {
        case GuiElementTypes::Text:
        {
            auto text = guiElementFactory_->CreateGuiText("new GuiText");
            guiManager_->Add(currentLayer_, std::move(text));
        }
        break;
        case GameEngine::GuiElementTypes::Texture:
        {
            auto texture = guiElementFactory_->CreateGuiTexture("new GuiTexture");
            guiManager_->Add(currentLayer_, std::move(texture));
        }
        break;
        case GameEngine::GuiElementTypes::Button:
        {
            auto button = guiElementFactory_->CreateGuiButton("new GuiButton",
                                                              [](auto&) { DEBUG_LOG("action not implemented."); });
            guiManager_->Add(currentLayer_, std::move(button));
        }
        break;
        case GameEngine::GuiElementTypes::Window:
        {
            auto window = guiElementFactory_->CreateGuiWindow(GameEngine::GuiWindowStyle::CLOSE, vec2(0), vec2(0.5));
            guiManager_->Add(currentLayer_, std::move(window));
        }
        break;
        case GameEngine::GuiElementTypes::Checkbox:
        {
            guiElementFactory_->CreateMessageBox("Error", "Create Checkbox not imeplement.");
            DEBUG_LOG("Create Checkbox not imeplement.");
        }
        break;
        case GameEngine::GuiElementTypes::EditBox:
        {
            auto editBox = guiElementFactory_->CreateEditBox("new GuiEditBox");
            guiManager_->Add(currentLayer_, std::move(editBox));
        }
        break;
        case GameEngine::GuiElementTypes::ComboBox:
        {
            guiElementFactory_->CreateMessageBox("Error", "Create ComboBox not imeplement.");
            DEBUG_LOG("Create ComboBox not imeplement.");
        }
        break;
        case GameEngine::GuiElementTypes::TreeView:
        {
            guiElementFactory_->CreateMessageBox("Error", "Create TreeView not imeplement.");
            DEBUG_LOG("Create TreeView not imeplement.");
        }
        break;
        case GameEngine::GuiElementTypes::VerticalLayout:
        {
            auto layout = guiElementFactory_->CreateVerticalLayout();
            guiManager_->Add(currentLayer_, std::move(layout));
        }
        break;
        case GameEngine::GuiElementTypes::HorizontalLayout:
        {
            auto layout = guiElementFactory_->CreateHorizontalLayout();
            guiManager_->Add(currentLayer_, std::move(layout));
        }
        break;
    }
}
void GuiEditScene::ReadLastOpenedLocation()
{
    lastOpenedLocation_ = Utils::ReadFile(TEMP_FILE_NAME);
}
void GuiEditScene::SaveLastOpenedLocation()
{
    if (not lastOpenedLocation_.empty())
        Utils::WrtieToFile(TEMP_FILE_NAME, lastOpenedLocation_);
}
}  // namespace Editor
