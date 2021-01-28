#include "PhysicsScene.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ResourceUtils.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "SingleTon.h"

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
    object->GetTransform().SetPosition(pos);
    object->GetTransform().SetScale(scale);

    object->AddComponent<Components::RendererComponent>().AddModel(modelFilename);

    auto& shape = object->AddComponent<Shape>().SetSize(scale);
    shape.SetPostionOffset(shapePositionOffset);
    object->AddComponent<Components::Rigidbody>()
        .SetIsStatic(isStatic)
        .SetCollisionShape(Shape::name)
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
            AddPhysicObject<Components::BoxShape>("Meshes/SimpleCube.obj", vec3(x, 200, y), vec3(0), vec3(0), 1.f,
                                                     false);
        }
    }
}

void PhysicsScene::RemoveObjectsUnderYValue(float)
{
}

void PhysicsScene::KeyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

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
    CreateHeightMap("Textures/Terrain/HeightMaps/flat_height_map.png",
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

void PhysicsScene::PostInitialize()
{
}

std::unique_ptr<GameEngine::GameObject> PhysicsScene::CreateGameObjectInstance(float scale, const vec2& position, bool)
{
    auto obj = CreateGameObject();
    vec3 obj_pos(position.x, 0, position.y);
    obj->GetTransform().SetPosition(obj_pos);
    obj->GetTransform().SetScale(scale);
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
                AddPhysicObject<Components::BoxShape>("Meshes/SimpleCube.obj", pos + vec3(i, 2 + k + 50, j), vec3(0),
                                                         vec3(0), 1.f, false);
            }
        }
    }
}

void PhysicsScene::AddExampleMesh(const vec3& pos, float scale)
{
    auto object = CreateGameObject();
    object->GetTransform().SetScale(scale);
    object->GetTransform().SetPosition(pos);

    object->AddComponent<Components::RendererComponent>().AddModel("Meshes/Rampa.obj");

    object->AddComponent<Components::MeshShape>().SetSize(scale);
    object->AddComponent<Components::Rigidbody>().SetIsStatic(true).SetCollisionShape(Components::MeshShape::name);

    AddGameObject(std::move(object));
}

int PhysicsScene::Update(float)
{
    UpdateObjectsCountText();
    RemoveObjectsUnderYValue(-100);
    return 0;
}
}  // namespace PhysicsTestGame
