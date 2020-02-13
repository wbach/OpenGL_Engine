#include "EditorScene.h"

#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>

#include "Editor/Context.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"

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
    // resourceManager_->GetGraphicsApi().SetBackgroundColor(context_.backgorundColor);

    // const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/SouthPool/SouthPool.xml");
    // LoadFromFile(sceneFile);

    for (uint32 i = 0; i < 4; ++i)
    {
        auto go = CreateGameObject("GameObjectName_" + std::to_string(i));
        go->AddComponent<GameEngine::Components::BoxShape>();
        go->AddComponent<GameEngine::Components::MeshShape>();

        AddGameObjects(go.get(), 3);
        AddGameObject(go);
    }

    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::W, [&]() {
        for (auto& go : gameObjects)
        {
            go.second->worldTransform.IncrasePosition(vec3(0.001));
        }
    });
    return 0;
}
void EditorScene::PostInitialize()
{
}
int EditorScene::Update(float)
{
    for (auto& go : gameObjects)
    {
        go.second->worldTransform.IncrasePosition(vec3(0.00001));
    }

    return 0;
}

void EditorScene::AddGameObjects(GameObject* go, int lvl)
{
    if (lvl == 0)
    {
        return;
    }

    for (uint32 i = 0; i < 4; ++i)
    {
        auto newGo = CreateGameObject(go->GetName() + "_" + std::to_string(i));
        newGo->AddComponent<GameEngine::Components::BoxShape>();
        newGo->AddComponent<GameEngine::Components::MeshShape>();

        AddGameObjects(newGo.get(), lvl - 1);
        go->AddChild(std::move(newGo));
    }
}
}  // namespace Editor
