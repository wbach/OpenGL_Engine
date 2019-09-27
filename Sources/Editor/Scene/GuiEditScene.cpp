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
#include <GameEngine/Renderers/GUI/Window/GuiWindow.h>
#include <Input/InputManager.h>
#include <algorithm>
#include "FileExplorer.h"
#include "FileSystem/FileSystemUtils.hpp"

using namespace GameEngine;

namespace Editor
{
namespace
{
const std::string NEW_LAYER_NAME = "NewLayer";
}
GuiEditScene::GuiEditScene()
    : GameEngine::Scene("GuiEditScene")
    , mousePosition_(0)
    , notCleanLayers_{"DefaultLayer"}
    , multiSelect_{false}
{
}
GuiEditScene::~GuiEditScene()
{
}
int GuiEditScene::Initialize()
{
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
    auto position  = inputManager_->GetMousePosition();
    auto mouseVec  = position - mousePosition_;
    mousePosition_ = position;

    if (not multiSelect_)
    {
        for (auto& element : guiElementsChoose_)
        {
            const auto& pos = element->GetPosition();
            element->SetPostion(pos + mouseVec);
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
    guiManager_->RegisterAction("FillGuiElementTypes()", [&](auto& element) {
        if (element.GetType() == GuiElementTypes::VerticalLayout or element.GetType() == GuiElementTypes::HorizontalLayout)
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
                    auto button = guiElementFactory_->CreateGuiButton(std::to_string(type), [this, type, &element](auto&) {
                        ShowCreateWindow(type);
                        guiManager_->AddTask([&]() { guiManager_->Remove(element); });
                    });
                    button->SetIsInternal(true);
                    layout->AddChild(std::move(button));
                }
            }
        }
    });
}

void GuiEditScene::AddMenuButtonAction()
{
    guiManager_->RegisterAction("ReadFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) {
            guiManager_->RemoveLayersExpect(notCleanLayers_);
            processingFilename_ = str;
            currentLayer_       = str;

            return guiElementFactory_->ReadGuiFile(str);
        });
    });

    guiManager_->RegisterAction("SaveToFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) { return guiManager_->SaveToFile(str, currentLayer_); });
    });

    guiManager_->RegisterAction("QuickSave()", [&](auto&) { guiManager_->SaveToFile(processingFilename_, currentLayer_); });

    guiManager_->RegisterAction("Reload()", [&](auto&) {
        guiManager_->RemoveLayersExpect(notCleanLayers_);
        guiElementFactory_->ReadGuiFile(processingFilename_);
    });

    guiManager_->RegisterAction("Clear()", [&](auto&) {
        currentLayer_.clear();
        guiManager_->RemoveLayersExpect(notCleanLayers_);
    });

    guiManager_->RegisterDefaultAction([&](auto& element) {
        auto id = element.GetId();

        auto existElement = std::find_if(guiElementsChoose_.begin(), guiElementsChoose_.end(), [id](auto el) { return el->GetId() == id; });
        if (existElement == guiElementsChoose_.end())
        {
            guiElementsChoose_.push_back(&element);
        }
    });

    guiManager_->RegisterAction("AddElement()", [&](auto&) {
        auto fullName = EngineConf_GetFullDataPath("Scenes/Editor/chooseComponentType.xml");
        guiElementFactory_->ReadGuiFile(fullName);
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
}

void GuiEditScene::EnableAlginToFirstX()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::X, [&]() {
        if (not guiElementsChoose_.empty())
        {
            auto x = guiElementsChoose_[0]->GetPosition().x;

            for (auto& element : guiElementsChoose_)
            {
                auto pos = element->GetPosition();
                pos.x    = x;
                element->SetPostion(pos);
            }
        }
    });
}

void GuiEditScene::EnableSetTheSameYDistance()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::Y, [&]() {
        if (guiElementsChoose_.size() > 2)
        {
            auto distance = guiElementsChoose_[1]->GetPosition().y - guiElementsChoose_[0]->GetPosition().y;

            for (size_t i = 2; i < guiElementsChoose_.size(); ++i)
            {
                auto pos = guiElementsChoose_[i]->GetPosition();
                pos.y    = guiElementsChoose_[i - 1]->GetPosition().y + distance;
                guiElementsChoose_[i]->SetPostion(pos);
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
            auto button = guiElementFactory_->CreateGuiButton("new GuiButton", [](auto&) { DEBUG_LOG("action not implemented."); });
            guiManager_->Add(currentLayer_, std::move(button));
        }
        break;
        case GameEngine::GuiElementTypes::Window:
        {
            auto window = guiElementFactory_->CreateGuiWindow(vec2(0), vec2(0.5), "new GuiWindow");
            guiManager_->Add(currentLayer_, std::move(window));
        }
        break;
        case GameEngine::GuiElementTypes::Checkbox:
        {
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
            DEBUG_LOG("Create ComboBox not imeplement.");
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
}  // namespace Editor
