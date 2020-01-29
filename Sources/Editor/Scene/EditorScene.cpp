#include "EditorScene.h"
#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include "Editor/Context.h"

using namespace GameEngine;

namespace Editor
{
EditorScene::EditorScene(Context& context)
    : GameEngine::Scene("EditorScene")
    , context_(context)
{
}
EditorScene::~EditorScene()
{
}
int EditorScene::Initialize()
{
    RunNetworkEditorInterface();
    //resourceManager_->GetGraphicsApi().SetBackgroundColor(context_.backgorundColor);

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/SouthPool/SouthPool.xml");
    LoadFromFile(sceneFile);

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    return 0;
}
int EditorScene::Update()
{
    return 0;
}
}  // namespace Editor
