#include "ChooseEditor.h"
#include <GameEngine/Engine/Configuration.h>
#include <GameEngine/Renderers/GUI/TreeView/TreeView.h>
#include <GameEngine/Renderers/GUI/Button/GuiButton.h>
#include <Input/InputManager.h>

using namespace GameEngine;

namespace Editor
{
ChooseEditor::ChooseEditor()
    : GameEngine::Scene("ChooseEditor")
{
}
int ChooseEditor::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

    guiManager_->RegisterAction("StartSceneEditor()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_NAME, "EditorScene");
        addSceneEvent(sceneEvent);
    });

    guiManager_->RegisterAction("StartGuiEditor()", [&](auto&) {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_NAME, "GuiEditScene");
        addSceneEvent(sceneEvent);
    });

    const std::string GUI_FILE = EngineConf_GetFullDataPath("Scenes/Editor/MainMenuGui.xml");
    guiElementFactory_->ReadGuiFile(GUI_FILE);

    guiManager_->RegisterAction("PrintSelectedTreeElement()", [&](auto& element) {
        DEBUG_LOG(element.GetLabel());
    });

    return 0;
}
}  // namespace Editor
