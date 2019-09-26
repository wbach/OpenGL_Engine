#include "GuiEditScene.h"
#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include <algorithm>
#include "FileExplorer.h"
#include "FileSystem/FileSystemUtils.hpp"

using namespace GameEngine;

namespace Editor
{
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

void GuiEditScene::AddMenuButtonAction()
{
    guiManager_->RegisterAction("ReadFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) {
            guiManager_->RemoveLayersExpect(notCleanLayers_);
            processingFilename_ = str;
            return guiElementFactory_->ReadGuiFile(str);
        });
    });

    guiManager_->RegisterAction("SaveToFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) { return guiManager_->SaveToFile(str, processingFilename_); });
    });

    guiManager_->RegisterAction("Clear()", [&](auto&) { guiManager_->RemoveLayersExpect(notCleanLayers_); });

    guiManager_->RegisterDefaultAction([&](auto& element) {
        auto id = element.GetId();

        auto existElement = std::find_if(guiElementsChoose_.begin(), guiElementsChoose_.end(), [id](auto el) { return el->GetId() == id; });
        if (existElement == guiElementsChoose_.end())
        {
            guiElementsChoose_.push_back(&element);
        }
    });
}

void GuiEditScene::ReadGuiFile(const std::string & file)
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

            for(size_t i = 2; i < guiElementsChoose_.size(); ++i)
            {
                auto pos = guiElementsChoose_[i]->GetPosition();
                pos.y = guiElementsChoose_[i - 1]->GetPosition().y + distance;
                guiElementsChoose_[i]->SetPostion(pos);
            }
        }
    });
}
}  // namespace Editor
