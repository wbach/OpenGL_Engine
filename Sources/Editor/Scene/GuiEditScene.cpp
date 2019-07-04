#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include "GuiEditScene.h"
#include "FileExplorer.h"

using namespace GameEngine;

namespace Editor
{
GuiEditScene::GuiEditScene()
    : GameEngine::Scene("GuiEditScene")
{
}
GuiEditScene::~GuiEditScene()
{
}
int GuiEditScene::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);

    guiManager_->RegisterAction("ReadFile()", [&]() {
        fileExplorer_->Start(".", [&](const std::string& str){guiElementFactory_->ReadGuiFile(str);});
    });

    guiManager_->RegisterAction("SaveToFile()", [&]() {
        fileExplorer_->Start(".", [&](const std::string& str){guiManager_->SaveToFile(str);});
    });

    const std::string GUI_FILE = EngineConf_GetFullDataPath("Scenes/Editor/CommonMenu.xml");
    guiElementFactory_->ReadGuiFile(GUI_FILE);

    return 0;
}
int GuiEditScene::Update()
{
    return 0;
}
}  // namespace Editor
