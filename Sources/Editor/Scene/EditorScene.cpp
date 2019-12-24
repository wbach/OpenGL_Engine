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
    resourceManager_->GetGraphicsApi().SetBackgroundColor(context_.backgorundColor);
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    return 0;
}
int EditorScene::Update()
{
    return 0;
}
}  // namespace Editor