#include "EditorScene.h"

#include <GameEngine/Engine/Configuration.h>
#include <Input/InputManager.h>

#include "Editor/Context.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Renderers/RenderersManager.h"

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

    const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/TestSene.xml");
    LoadFromFile(sceneFile);

    camera.SetPosition(vec3(2, 2, 2));
    camera.LookAt(vec3(0, 0.5, 0));
    camera.UpdateMatrix();

    const auto& terrainShapeComponents =
        componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainShape);

    vec3 cratePosition(0);

    DEBUG_LOG("terrainShapeComponents.count : " + std::to_string(terrainShapeComponents.size()));

    for (const auto& p : terrainShapeComponents)
    {
        auto terrainShapeComponent = static_cast<Components::TerrainShape*>(p.second);

        if (terrainShapeComponent)
        {
            auto terrainHeight = terrainShapeComponent->GetHeightOfTerrain(cratePosition);

            if (terrainHeight)
            {
                cratePosition.y = *terrainHeight;
                DEBUG_LOG("Succes, get terrain height : " + std::to_string(cratePosition));
                break;
            }
            else
            {
                DEBUG_LOG("outside of terrain.");
            }
        }
        else
        {
            DEBUG_LOG("cast error.count ");
        }
    }

    DEBUG_LOG("Crate pos : " + std::to_string(cratePosition));

    {
        auto go = CreateGameObject("Crate");
        go->AddComponent<GameEngine::Components::RendererComponent>().AddModel("Meshes/Crate/crate.obj",
                                                                               GameEngine::LevelOfDetail::L1);
        go->worldTransform.SetPosition(cratePosition);
        go->worldTransform.SetRotation(DegreesVec3(0, 45, 0));
        go->worldTransform.TakeSnapShoot();
        AddGameObject(go);
    }

    // for (uint32 i = 0; i < 4; ++i)
    //{
    //    auto go = CreateGameObject("GameObjectName_" + std::to_string(i));
    //    go->AddComponent<GameEngine::Components::BoxShape>();
    //    go->AddComponent<GameEngine::Components::MeshShape>();

    //    AddGameObjects(go.get(), 3);
    //    AddGameObject(go);
    //}

    KeySubscribtions();

    return 0;
}
void EditorScene::PostInitialize()
{
}
int EditorScene::Update(float)
{
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

void EditorScene::KeySubscribtions()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->DisableDebugRenderer(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->EnableDebugRenderer(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });
    inputManager_->SubscribeOnKeyDown(
        KeyCodes::L, [renderersManager = this->renderersManager_]() { renderersManager->SwapLineFaceRender(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::W, [&]() {
        for (auto& go : gameObjects)
        {
            go->worldTransform.IncrasePosition(vec3(0.001));
        }
    });
}
}  // namespace Editor
