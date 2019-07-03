#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>
#include "GuiEditScene.h"

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
    return 0;
}
int GuiEditScene::Update()
{
    return 0;
}
}  // namespace Editor