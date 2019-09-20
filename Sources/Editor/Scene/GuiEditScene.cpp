#include "GuiEditScene.h"
#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include "FileExplorer.h"
#include "FileSystem/FileSystemUtils.hpp"
#include <algorithm>

using namespace GameEngine;

namespace Editor
{
namespace
{
std::string GUI_FILE;
std::vector<GuiElement*> guiElementsChoose;
bool multiSelect{false};
}  // namespace

GuiEditScene::GuiEditScene()
    : GameEngine::Scene("GuiEditScene")
    , mousePosition_(0)
{
}
GuiEditScene::~GuiEditScene()
{
}
int GuiEditScene::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

    guiManager_->RegisterAction("ReadFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) {
            guiManager_->RemoveNotPermaments();
            return guiElementFactory_->ReadGuiFile(str);
        });
    });

    guiManager_->RegisterAction("SaveToFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) { return guiManager_->SaveToFile(str); });
    });

    guiManager_->RegisterDefaultAction([&](auto& element) {
        DEBUG_LOG("Add element");
        auto id = element.GetId();

        auto existElement = std::find_if(guiElementsChoose.begin(), guiElementsChoose.end(), [id](auto el) { return el->GetId() == id; });
        if (existElement == guiElementsChoose.end())
        {
            guiElementsChoose.push_back(&element);
        }
    });

    inputManager_->SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() {
        DEBUG_LOG("remove elements");
        if (not multiSelect)
        {
            guiElementsChoose.clear();
        }
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::LCTRL, [&]() { multiSelect = true; });
    inputManager_->SubscribeOnKeyUp(KeyCodes::LCTRL, [&]() { multiSelect = false; });

    GUI_FILE = EngineConf_GetFullDataPath("Scenes/Editor/CommonMenu.xml");
    guiElementFactory_->ReadGuiFile(GUI_FILE);

    return 0;
}
int GuiEditScene::Update(float)
{
    auto position  = inputManager_->GetMousePosition();
    auto mouseVec  = position - mousePosition_;
    mousePosition_ = position;

    if (not multiSelect)
    {
        for (auto& element : guiElementsChoose)
        {
            DEBUG_LOG("Move element");
            const auto& pos = element->GetPosition();
            element->SetPostion(pos + mouseVec);
        }
    }
    return 0;
}
}  // namespace Editor
