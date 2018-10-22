#include "PhysicsScene.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/TreeRendererComponent.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainWrapper.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiText.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "SingleTon.h"

#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/TerrainShape.h"

#include "GLM/GLMUtils.h"
#include "GameEngine/Api/BlendFunctionsTypes.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"
#include "Thread.hpp"

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5

using namespace GameEngine;

namespace PhysicsTestGame
{
TerrainTexturesMap CreateTerrainTexturesMap()
{
    return {{TerrainTextureType::blendMap, "Textures/Terrain/BlendMaps/testBlendMap.png"},
            {TerrainTextureType::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png"},
            {
                TerrainTextureType::redTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png",
            },
            {
                TerrainTextureType::rockTexture, "Textures/Terrain/Ground/G3_Nature_Wall_Stone_12_Diffuse_01.png",
            },
            {
                TerrainTextureType::snowTexture, "Textures/Terrain/Ground/snow512.png",
            },
            {TerrainTextureType::greenTexture, "Textures/Terrain/Ground/grassFlowers.png"},
            {TerrainTextureType::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png"},
            {TerrainTextureType::displacementMap, "Textures/Terrain/HeightMaps/TerrainFlat.terrain"}};
}

const float SEARCH_DEBUG = 1000;

PhysicsScene::PhysicsScene()
    : Scene("PhysicsScene")
{
}

PhysicsScene::~PhysicsScene()
{
    Log(__FUNCTION__);
}

float Random()
{
    return static_cast<float>(rand() % 100000) / 100000.f;
}

void PhysicsScene::AddBox(const vec3& pos, const vec3& dir, float scale, bool isStatic)
{
    auto object = CreateGameObject(scale, pos);

    AddComponent<Components::RendererComponent>(object)->AddModel("Meshes/SimpleCube.obj");

    auto boxShape = AddComponent<Components::BoxShape>(object);
    boxShape->SetSize(scale / 2.f);

    auto rigidbody = AddComponent<Components::Rigidbody>(object);

    rigidbody->SetIsStatic(isStatic);
    rigidbody->SetCollisionShape(boxShape);

    if (isStatic)
    {
        rigidbody->SetMass(0.f);
    }

    AddGameObject(object);
    rigidbody->SetVelocity(dir);
}

void PhysicsScene::AddSphere(const vec3& pos, const vec3& dir, float scale, bool isStatic)
{
    auto object = CreateGameObject(scale, pos);

    AddComponent<Components::RendererComponent>(object)->AddModel("Meshes/sphere.obj");

    auto boxShape = AddComponent<Components::SphereShape>(object);
    boxShape->SetSize(scale / 2.f);

    auto rigidbody = AddComponent<Components::Rigidbody>(object);

    rigidbody->SetIsStatic(isStatic);
    rigidbody->SetCollisionShape(boxShape);

    if (isStatic)
    {
        rigidbody->SetMass(0.f);
    }

    AddGameObject(object);
    rigidbody->SetVelocity(dir);
}

const std::string OBJECT_COUNT_GUI_TEXT = "objectsCount";

void PhysicsScene::UpdateObjectsCountText()
{
    renderersManager_->GuiText(OBJECT_COUNT_GUI_TEXT).text = "Objects count : " + std::to_string(gameObjects.size());
}

void PhysicsScene::AddDebuxBoxesPlane(const vec2& offset)
{
    for (int y = offset.y; y < offset.y + 50; y += 2)
    {
        for (int x = offset.x; x < offset.x + 50; x += 2)
        {
            AddBox(vec3(x, 200, y), vec3(0), 1.f, false);
        }
    }
}

void PhysicsScene::RemoveObjectsUnderYValue(float y)
{
    for (auto iter = objects_.begin(); iter != objects_.end();)
    {
        if ((*iter)->worldTransform.GetPosition().y < -100)
        {
            RemoveGameObject(*iter);
            iter = objects_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void PhysicsScene::KeyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::F, [&]() {
        auto dir = GetCamera()->GetDirection();
        dir      = glm::normalize(dir);
        auto pos = GetCamera()->GetPosition();
        AddSphere(pos, dir * 20.f, 1.0f);
        Log("Dir : " + Utils::ToString(dir) + ", Pos : " + Utils::ToString(pos) +
            ", Objecsts : " + std::to_string(objects_.size()));
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::T, [&]() { simulatePhysics_.store(!simulatePhysics_.load()); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::R, [&]() {
        for (auto iter = objects_.begin(); iter != objects_.end();)
        {
            RemoveGameObject(*iter);
            iter = objects_.erase(iter);
        }
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::B, [&]() { AddBoxes(GetCamera()->GetPosition()); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::G, [&]() { AddBarrel(GetCamera()->GetPosition()); });

    inputManager_->SubscribeOnKeyDown(
        KeyCodes::P, [&]() { Log("Camera position : " + Utils::ToString(GetCamera()->GetPosition())); });
}

int PhysicsScene::Initialize()
{
    resourceManager_->GetTextureLaoder().CreateHeightMap("Textures/Terrain/HeightMaps/flat_height_map.png",
                                                         "Textures/Terrain/HeightMaps/TerrainFlat.terrain");
    auto& octext    = renderersManager_->GuiText(OBJECT_COUNT_GUI_TEXT);
    octext.position = vec2(0.5, 0.9);
    octext.m_size   = 0.5f;
    octext.colour   = vec3(1, 0, 0);
    UpdateObjectsCountText();

    camera = std::make_unique<FirstPersonCamera>(inputManager_, displayManager_);
    camera->SetPosition(vec3(-0, 42, 0));

    AddBarrel(vec3(0, 42, 0));

    //{
    //	auto object = CreateGameObject(1, vec3(512, 0, 0));
    //	AddComponent<Components::RendererComponent>(object)->AddModel("Meshes/SimpleCube.obj");
    //	AddGameObject(object);
    //}

    //{
    //	auto object = CreateGameObject(1, vec3(512, 0, 512));
    //	AddComponent<Components::RendererComponent>(object)->AddModel("Meshes/SimpleCube.obj");
    //	AddGameObject(object);
    //}

    //{
    //	auto object = CreateGameObject(1, vec3(0, 0, 0));
    //	AddComponent<Components::RendererComponent>(object)->AddModel("Meshes/SimpleCube.obj");
    //	AddGameObject(object);
    //}

    //{
    //	auto object = CreateGameObject(1, vec3(0, 0, 512));
    //	AddComponent<Components::RendererComponent>(object)->AddModel("Meshes/SimpleCube.obj");
    //	AddGameObject(object);
    //}
    // AddBoxes();

    // float dSize = 100;
    // AddBox(vec3(0, 0, 0), vec3(0), dSize, true);

    auto terrain_textures = CreateTerrainTexturesMap();
    auto terrain          = AddTerrain(terrain_textures, glm::vec3(1));
    terrain->GetHeight(0, 0);

    {
        auto terrainShapeComponent = AddComponent<Components::TerrainShape>(terrain);
        terrainShapeComponent->SetSize(terrain->GetSize());
        terrainShapeComponent->SetData(&terrain->GetHeightData());
        terrainShapeComponent->SetHeightFactor(1.f);
        terrainShapeComponent->SetPostionOffset(vec3(0, 10, 0));

        auto rigidbody = AddComponent<Components::Rigidbody>(terrain);
        rigidbody->SetCollisionShape(terrainShapeComponent);
        rigidbody->SetIsStatic(true);
        rigidbody->SetMass(0.f);
    }
    AddGameObject(terrain, glm::vec3(1));

    KeyOperations();

    return 0;
}

Terrain* PhysicsScene::AddTerrain(TerrainTexturesMap& textures, const glm::vec3&)
{
    auto terrain = ObjectBuilder::CreateTerrain(resourceManager_.get(), textures);
    if (terrain == nullptr)
    {
        Error("MainScene::AddTerrain : terrain is nullptr.");
        return nullptr;
    }
    /*AddGameObject(terrain, position);*/
    renderersManager_->Subscribe(terrain);
    terrains_.push_back(terrain);

    return static_cast<TerrainWrapper*>(terrain)->Get();
}

GameObject* PhysicsScene::CreateGameObject(float scale, const vec3& position, bool isDynamic)
{
    auto obj = new GameObject();
    obj->worldTransform.SetPosition(position);
    obj->worldTransform.SetRotation(vec3(0.0f));
    obj->worldTransform.SetScale(scale);
    obj->worldTransform.isDynamic_ = isDynamic;
    obj->worldTransform.TakeSnapShoot();
    objects_.push_back(obj);
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
                AddBox(pos + vec3(i, 2 + k + 50, j), vec3(0), 1.f, false);
            }
        }
    }
}

void PhysicsScene::AddBarrel(const vec3& pos)
{
    auto object = CreateGameObject(10.f, pos);

    auto renderComponent = AddComponent<Components::RendererComponent>(object);
    renderComponent->AddModel("Meshes/Rampa.obj");

    auto meshShape = AddComponent<Components::MeshShape>(object);
    meshShape->SetModel(renderComponent->GetModelWrapper().Get(LevelOfDetail::L1));

    auto rigidbody = AddComponent<Components::Rigidbody>(object);

    rigidbody->SetIsStatic(false);
    rigidbody->SetCollisionShape(meshShape);

    AddGameObject(object);
}

int PhysicsScene::Update(float dt)
{
    if (camera == nullptr)
    {
        Log("PhysicsScene::Update camera is nullptr.");
        return -1;
    }

    UpdateObjectsCountText();
    RemoveObjectsUnderYValue(-100);
    return 0;
}
}  // PhysicsTestGame
