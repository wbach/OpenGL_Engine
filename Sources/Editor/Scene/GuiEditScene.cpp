#include "GuiEditScene.h"
#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include <algorithm>
#include "FileExplorer.h"
#include "FileSystem/FileSystemUtils.hpp"

using namespace GameEngine;

namespace Editor
{
namespace
{
std::string GUI_FILE = "";
std::vector<GuiElement*> guiElementsChoose;
std::string processingFilename;
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
    GUI_FILE = EngineConf_GetFullDataPath("Scenes/Editor/CommonMenu.xml");

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

    guiManager_->RegisterAction("ReadFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str) {
            guiManager_->RemoveLayersExpect({GUI_FILE, "DefaultLayer"});
            processingFilename = str;
            return guiElementFactory_->ReadGuiFile(str);
        });
    });

    guiManager_->RegisterAction("SaveToFile()", [&](auto&) {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(),
                             [&](const std::string& str) { return guiManager_->SaveToFile(str, processingFilename); });
    });

    guiManager_->RegisterAction("Clear()", [&](auto&) {
        guiManager_->RemoveLayersExpect({ GUI_FILE, "DefaultLayer" });
    });

    guiManager_->RegisterDefaultAction([&](auto& element) {
        DEBUG_LOG("Add element");
        auto id = element.GetId();

        auto existElement = std::find_if(guiElementsChoose.begin(), guiElementsChoose.end(),
                                         [id](auto el) { return el->GetId() == id; });
        if (existElement == guiElementsChoose.end())
        {
            guiElementsChoose.push_back(&element);
        }
    });

    inputManager_->SubscribeOnKeyUp(KeyCodes::LMOUSE, [&]() {
        if (not multiSelect)
        {
            guiElementsChoose.clear();
        }
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::LCTRL, [&]() { multiSelect = true; });
    inputManager_->SubscribeOnKeyUp(KeyCodes::LCTRL, [&]() { multiSelect = false; });

    guiElementFactory_->ReadGuiFile(GUI_FILE);

    for (auto& element : guiManager_->GetLayer(GUI_FILE)->GetElements())
    {
        element->SetZPosition(-1.f);
    }

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
            const auto& pos = element->GetPosition();
            element->SetPostion(pos + mouseVec);
        }
    }
    return 0;
}
}  // namespace Editor
