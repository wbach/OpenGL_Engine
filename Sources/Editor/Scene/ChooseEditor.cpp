#include "ChooseEditor.h"
#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>

using namespace GameEngine;

namespace Editor
{
ChooseEditor::ChooseEditor()
    : GameEngine::Scene("ChooseEditor")
{
}
ChooseEditor::~ChooseEditor()
{
}
int ChooseEditor::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

    guiManager_->RegisterAction("StartSceneEditor()", [&]() {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_NAME, "EditorScene");
        addSceneEvent(sceneEvent);
    });

    guiManager_->RegisterAction("StartGuiEditor()", [&]() {
        SceneEvent sceneEvent(SceneEventType::LOAD_SCENE_BY_NAME,  "GuiEditScene");
        addSceneEvent(sceneEvent);
    });

    const std::string GUI_FILE = EngineConf_GetFullDataPath("Scenes/Editor/MainMenuGui.xml");
    guiElementFactory_->ReadGuiFile(GUI_FILE);
    return 0;
}
int ChooseEditor::Update()
{
    return 0;
}
}  // namespace Editor