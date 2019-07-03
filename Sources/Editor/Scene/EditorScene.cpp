#include "EditorScene.h"
#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>

using namespace GameEngine;

namespace Editor
{
EditorScene::EditorScene()
    : GameEngine::Scene("EditorScene")
{
}
EditorScene::~EditorScene()
{
}
int EditorScene::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    return 0;
}
int EditorScene::Update()
{
    return 0;
}
}  // namespace Editor