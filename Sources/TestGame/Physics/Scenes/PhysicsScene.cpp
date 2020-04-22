#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "PhysicsScene.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "SingleTon.h"

#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"

#include "GLM/GLMUtils.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GraphicsApi/BlendFunctionsTypes.h"
#include "Thread.hpp"

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

using namespace GameEngine;

namespace PhysicsTestGame
{
TerrainTexturesFilesMap CreateTerrainTexturesMap()
{
    // clang-format off
    return
    {
        {TerrainTextureType::blendMap, "Textures/Terrain/BlendMaps/testBlendMap.png"},
        {TerrainTextureType::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png"},
        {TerrainTextureType::redTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        {TerrainTextureType::greenTexture, "Textures/Terrain/Ground/grassFlowers.png"},
        {TerrainTextureType::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png"},
        {TerrainTextureType::heightmap, "Textures/Terrain/HeightMaps/TerrainFlat.terrain"}
    };
    // clang-format on
}

const float SEARCH_DEBUG = 1000;

PhysicsScene::PhysicsScene()
    : Scene("PhysicsScene")
{
}

PhysicsScene::~PhysicsScene()
{
    DEBUG_LOG("");
}

float Random()
{
    return static_cast<float>(rand() % 100000) / 100000.f;
}

template <typename Shape>
void PhysicsScene::AddPhysicObject(const std::string& modelFilename, const vec3& pos, const vec3& shapePositionOffset,
                                   const vec3& dir, float scale, bool isStatic)
{
    auto object = CreateGameObject();
    object->worldTransform.SetPosition(pos);
    object->worldTransform.SetScale(scale);
    object->worldTransform.TakeSnapShoot();
    object->AddComponent<Components::RendererComponent>().AddModel(modelFilename);

    auto& shape = object->AddComponent<Shape>().SetSize(scale);
    shape.SetPostionOffset(shapePositionOffset);
    object->AddComponent<Components::Rigidbody>()
        .SetIsStatic(isStatic)
        .SetCollisionShape(shape.GetType())
        .SetVelocity(dir);

    AddGameObject(std::move(object));
}

const std::string OBJECT_COUNT_GUI_TEXT = "objectsCount";

void PhysicsScene::UpdateObjectsCountText()
{
    // renderersManager_->GuiText(OBJECT_COUNT_GUI_TEXT).text = "Objects count : " + std::to_string(gameObjects.size());
}

void PhysicsScene::AddDebuxBoxesPlane(const vec2& offset)
{
    for (int y = static_cast<int>(offset.y); y < static_cast<int>(offset.y) + 50; y += 2)
    {
        for (int x = static_cast<int>(offset.x); x < static_cast<int>(offset.x) + 50; x += 2)
        {
            AddPhysicObject<Components::SphereShape>("Meshes/SimpleCube.obj", vec3(x, 200, y), vec3(0), vec3(0), 1.f,
                                                     false);
        }
    }
}

void PhysicsScene::RemoveObjectsUnderYValue(float y)
{
}

void PhysicsScene::KeyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&](){ addEngineEvent(EngineEvent::QUIT);});

    inputManager_->SubscribeOnKeyDown(KeyCodes::F, [&]() {
        auto dir = GetCamera().GetDirection();
        dir      = glm::normalize(dir);
        auto pos = GetCamera().GetPosition();
        AddPhysicObject<Components::SphereShape>("Meshes/sphere.obj", pos, vec3(0), dir * 20.f, 1.f, false);
        DEBUG_LOG("Dir : " + std::to_string(dir) + ", Pos : " + std::to_string(pos) +
                  ", Objecsts : " + std::to_string(GetAllGameObjectsPtrs().size()));
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::T, [&]() { simulatePhysics_.store(!simulatePhysics_.load()); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::R, [&]() {
        ClearGameObjects();
        AddStartupObjects();
    });
    inputManager_->SubscribeOnKeyDown(KeyCodes::B, [&]() { AddBoxes(GetCamera().GetPosition()); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::L, [&]() { renderersManager_->SwapLineFaceRender(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Disable(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::G, [&]() { AddExampleMesh(GetCamera().GetPosition(), 10.f); });

    inputManager_->SubscribeOnKeyDown(
        KeyCodes::P, [&]() { DEBUG_LOG("Camera position : " + std::to_string(GetCamera().GetPosition())); });
}
void PhysicsScene::AddStartupObjects()
{
    AddPhysicObject<Components::SphereShape>("Meshes/sphere.obj", vec3(0, 2, 10), vec3(0), vec3(0), 1.f, true);
    AddPhysicObject<Components::SphereShape>("Meshes/sphere.obj", vec3(10, 2, 10), vec3(0), vec3(0), 2.f, true);
    AddPhysicObject<Components::SphereShape>("Meshes/sphere.obj", vec3(20, 2, 10), vec3(0), vec3(0), 3.f, true);
    AddPhysicObject<Components::SphereShape>("Meshes/sphere.obj", vec3(30, 2, 10), vec3(0), vec3(0), 4.f, true);
    AddPhysicObject<Components::BoxShape>("Meshes/Crate/crate.obj", vec3(0, 2, 0), vec3(0, -.5f, 0), vec3(0), 1.f,
                                          true);
    AddPhysicObject<Components::BoxShape>("Meshes/Crate/crate.obj", vec3(10, 2, 0), vec3(0, -1.f, 0), vec3(0), 2.f,
                                          true);
    AddPhysicObject<Components::BoxShape>("Meshes/Crate/crate.obj", vec3(20, 2, 0), vec3(0, -1.5f, 0), vec3(0), 3.f,
                                          true);
    AddPhysicObject<Components::BoxShape>("Meshes/Crate/crate.obj", vec3(30, 2, 0), vec3(0, -2.f, 0), vec3(0), 4.f,
                                          true);
    AddExampleMesh(vec3(0, 2, 20), 1.f);
    AddExampleMesh(vec3(10, 2, 20), 2.f);
    AddExampleMesh(vec3(20, 2, 20), 3.f);
    AddExampleMesh(vec3(30, 2, 20), 4.f);

    AddTerrain();
}
void PhysicsScene::CreateAndAddGameEntity(const std::string& filename, float scale, const vec2& position,
                                          uint32_t textureIndex, bool isDynamic)
{
    auto object = CreateGameObjectInstance(scale, position, isDynamic);
    object->AddComponent<Components::RendererComponent>().AddModel(filename).SetTextureIndex(textureIndex);
    AddGameObject(std::move(object));
}
int PhysicsScene::Initialize()
{
    resourceManager_->GetTextureLaoder().CreateHeightMap("Textures/Terrain/HeightMaps/flat_height_map.png",
                                                         "Textures/Terrain/HeightMaps/TerrainFlat.terrain", vec3(1.f));
    // auto& octext    = renderersManager_->GuiText(OBJECT_COUNT_GUI_TEXT);
    // octext.position = vec2(0.5, 0.9);
    // octext.m_size   = 0.5f;
    // octext.colour   = vec3(1, 0, 0);
    UpdateObjectsCountText();

    camera_ = std::make_unique<FirstPersonCamera>(*inputManager_, *displayManager_);
    camera.Set(*camera_);
    camera.SetPosition(vec3(0, 5, -10));
    AddStartupObjects();

    KeyOperations();

    return 0;
}

void PhysicsScene::AddTerrain()
{
    auto textures = CreateTerrainTexturesMap();
    auto object   = CreateGameObjectInstance(1.f, vec2(0));
    resourceManager_->GetTextureLaoder().SetHeightMapFactor(10.f);
    object->AddComponent<Components::TerrainMeshRendererComponent>().LoadTextures(textures);

    auto& terrainShapeComponent =
        object->AddComponent<Components::TerrainShape>().SetHeightMap(textures.at(TerrainTextureType::heightmap));

    auto rigidbody = object->AddComponent<Components::Rigidbody>()
                         .SetCollisionShape(terrainShapeComponent.GetType())
                         .SetIsStatic(true);

    AddGameObject(std::move(object));
}

std::unique_ptr<GameEngine::GameObject> PhysicsScene::CreateGameObjectInstance(float scale, const vec2& position,
                                                                               bool isDynamic)
{
    auto obj = CreateGameObject();
    obj->worldTransform.SetScale(scale);
    vec3 obj_pos(position.x, 0, position.y);
    obj->worldTransform.SetPosition(obj_pos);
    obj->worldTransform.TakeSnapShoot();
    return obj;
}

void PhysicsScene::AddBoxes(const vec3& pos)
{
    for (int k = 0; k < ARRAY_SIZE_Y; k++)
    {
        for (int i = 0; i < ARRAY_SIZE_X; i++)
        {
            for (int j = 0; j < ARRAY_SIZE_Z; j++)
            {
                AddPhysicObject<Components::SphereShape>("Meshes/SimpleCube.obj", pos + vec3(i, 2 + k + 50, j), vec3(0),
                                                         vec3(0), 1.f, false);
            }
        }
    }
}

void PhysicsScene::AddExampleMesh(const vec3& pos, float scale)
{
    auto object = CreateGameObject();
    object->worldTransform.SetScale(scale);
    object->worldTransform.SetPosition(pos);
    object->worldTransform.TakeSnapShoot();

    object->AddComponent<Components::RendererComponent>().AddModel("Meshes/Rampa.obj");

    auto& meshShape = object->AddComponent<Components::MeshShape>().SetSize(scale);
    object->AddComponent<Components::Rigidbody>().SetIsStatic(true).SetCollisionShape(meshShape.GetType());

    AddGameObject(std::move(object));
}

int PhysicsScene::Update(float dt)
{
    UpdateObjectsCountText();
    RemoveObjectsUnderYValue(-100);
    return 0;
}
}  // namespace PhysicsTestGame
