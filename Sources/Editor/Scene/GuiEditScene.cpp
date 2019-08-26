#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include "GuiEditScene.h"
#include "FileExplorer.h"
#include "FileSystem/FileSystemUtils.hpp"

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

    guiManager_->RegisterAction("ReadFile()", [&]() {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str){ return guiElementFactory_->ReadGuiFile(str);});
    });

    guiManager_->RegisterAction("SaveToFile()", [&]() {
        fileExplorer_ = std::make_unique<FileExplorer>(*guiManager_, *guiElementFactory_);
        fileExplorer_->Start(Utils::GetCurrentDir(), [&](const std::string& str){ return guiManager_->SaveToFile(str);});
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [&](){
        DEBUG_LOG("Elements : ");
        for (const auto& el : guiManager_->GetElements())
        {
            DEBUG_LOG(" : Z offset : "+ std::to_string(el->GetZOffsetValue()) + ", Z position : " + std::to_string(el->GetZValue()) + ", z total : " + std::to_string(el->GetZTotalValue()));
        }
        DEBUG_LOG("========================");
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
