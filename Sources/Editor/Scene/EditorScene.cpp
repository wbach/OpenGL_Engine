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
    camera.SetPosition(vec3(2, 2, 2));
    camera.LookAt(vec3(0, 0.5, 0));
    camera.UpdateMatrix();

    RunNetworkEditorInterface();
    renderersManager_->GetDebugRenderer().Enable();

    //// resourceManager_->GetGraphicsApi().SetBackgroundColor(context_.backgorundColor);

    // const std::string sceneFile = EngineConf_GetFullDataPath("Scenes/TestSene.xml");
    // LoadFromFile(sceneFile);

     const auto& terrainShapeComponents =
        componentController_.GetAllComonentsOfType(Components::ComponentsType::TerrainShape);

     vec3 cratePosition(2, 0, 1);

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
        go->GetTransform().SetPosition(cratePosition);
        go->GetTransform().SetRotation(DegreesVec3(0, 45, 0));
        AddGameObject(std::move(go));
    }

    {
        auto go = CreateGameObject("Crate2");
        go->AddComponent<GameEngine::Components::RendererComponent>().AddModel("Meshes/Crate/crate.obj",
            GameEngine::LevelOfDetail::L1);
        go->GetTransform().SetPosition(vec3(0, 0, 0));
        go->GetTransform().SetRotation(DegreesVec3(0, 0, 0));
        AddGameObject(std::move(go));
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
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Disable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::ASK_QUIT); });
    inputManager_->SubscribeOnKeyDown(
        KeyCodes::L, [renderersManager = this->renderersManager_]() { renderersManager->SwapLineFaceRender(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::J, [&]() {
        for (auto& go : GetGameObjects())
        {
            go->GetTransform().IncrasePosition(vec3(0.001f));
        }
    });
}
}  // namespace Editor
