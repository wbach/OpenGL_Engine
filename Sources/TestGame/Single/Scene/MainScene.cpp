#include "MainScene.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Api/BlendFunctionsTypes.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainDef.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiText.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "SingleTon.h"
#include "Thread.hpp"

using namespace GameEngine;

TerrainTexturesFilesMap CreateTerrainTexturesMap()
{
    // clang-format off
    return
    {
        {TerrainTextureType::blendMap, "Textures/Terrain/BlendMaps/testBlendMap.png"},
        {TerrainTextureType::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png"},
        {TerrainTextureType::redTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        {TerrainTextureType::rockTexture, "Textures/Terrain/Ground/G3_Nature_Wall_Stone_12_Diffuse_01.png"},
        {TerrainTextureType::snowTexture, "Textures/Terrain/Ground/snow512.png"},
        {TerrainTextureType::greenTexture, "Textures/Terrain/Ground/grassFlowers.png"},
        {TerrainTextureType::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png"},
        {TerrainTextureType::displacementMap, "Textures/Terrain/HeightMaps/Terrain.terrain"}
    };
    // clang-format on
}

MainScene::MainScene()
    : Scene("MainScene")
{
}

MainScene::~MainScene()
{
    Log(__FUNCTION__);
}

float Random()
{
    return static_cast<float>(rand() % 100000) / 100000.f;
}

int MainScene::Initialize()
{
    Log("MainScene::Initialize()");

    renderersManager_->GuiText("playerPos").position = vec2(-0.9, -0.9);
    renderersManager_->GuiText("playerPos").m_size   = .5f;
    renderersManager_->GuiText("playerPos").colour   = vec3(.8f, 0.f, 0.f);
    renderersManager_->GuiText("gameTime").position  = vec2(0.75, 0.9);
    renderersManager_->GuiText("gameTime").m_size    = 0.5f;
    renderersManager_->GuiText("gameTime").text = "Game Time" + std::to_string(dayNightCycle.GetCurrentHour().x) + ":" +
                                                  std::to_string(dayNightCycle.GetCurrentHour().y);
    renderersManager_->GuiText("cameraPos").position = vec2(-0.9, -0.8);
    renderersManager_->GuiText("cameraPos").m_size   = .5f;
    renderersManager_->GuiText("cameraPos").colour   = vec3(.8f, 0.f, 0.f);

    // clang-format off
    {
        std::vector<std::string> nightTextures{ "Skybox/Night/right.png", "Skybox/Night/left.png",
                                                "Skybox/Night/top.png",   "Skybox/Night/bottom.png",
                                                "Skybox/Night/back.png",  "Skybox/Night/front.png" };

        std::vector<std::string> dayTextures{   "Skybox/TropicalSunnyDay/right.png", "Skybox/TropicalSunnyDay/left.png",
                                                "Skybox/TropicalSunnyDay/top.png",   "Skybox/TropicalSunnyDay/bottom.png",
                                                "Skybox/TropicalSunnyDay/back.png",  "Skybox/TropicalSunnyDay/front.png" };

        auto skybox = CreateGameObjectInstance(1.f, vec2(0));
        skybox->AddComponent<Components::SkyBoxComponent>()
            .SetModel("Meshes/SkyBox/cube.obj")
            .SetNightTexture(nightTextures)
            .SetDayTexture(dayTextures);

        AddGameObject(skybox);
    }
    // clang-format on

    auto terrain_textures = CreateTerrainTexturesMap();
    AddTerrain(terrain_textures, glm::vec3(1));

    {
        std::vector<vec3> treePositions;
        vec2ui size(10, 10);
        treePositions.resize(size.x * size.y);

        for (uint32 y = 0; y < size.y; y++)
        {
            for (uint32 x = 0; x < size.x; x++)
            {
                vec3 treePos(10.f * x, 0.f, 10.f * y);
                treePos.x += static_cast<float>(rand() % 100) / 10.f;
                treePos.z += static_cast<float>(rand() % 100) / 10.f;
                treePos = treePos + vec3(350, 0, 450);

                // for (auto& terrain : terrains_)
                //{
                //    auto new_position = terrain->CollisionDetection(treePos);
                //    if (!new_position)
                //        continue;

                //    treePos.y = new_position.constValue().y - .5f;
                //}
                treePositions[x + size.x * y] = treePos;
            }
        }
        auto tree1 = CreateGameObjectInstance(20.f, vec2(0, 0));
        tree1->AddComponent<Components::TreeRendererComponent>()
            .SetPositions(treePositions, size)
            .SetTopModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/top.obj")
            .SetBottomModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/bottom2T.obj");
        AddGameObject(tree1);
    }

    {
        auto particle1 = CreateGameObjectInstance(1.f, vec2(400, 560));

        Particle particle;
        particle.position      = particle1->worldTransform.GetPosition();
        particle.velocity      = vec3(0, 0.1, 0);
        particle.rotation      = 0;
        particle.scale         = 8;
        particle.gravityEffect = 1.f;
        particle.lifeTime      = 2.f;

        particle1->AddComponent<Components::ParticleEffectComponent>()
            .SetParticle(particle)
            .SetTexture("Textures/Particles/water.png")
            .SetParticlesPerSec(10)
            .SetBlendFunction(BlendFunctionType::ONE)
            .SetEmitFunction([](const Particle& referenceParticle) -> Particle {
                Particle particle = referenceParticle;

                float dirX        = Random() - 0.5f;
                float dirZ        = Random() - 0.5f;
                particle.velocity = vec3(dirX, 1, dirZ);

                return particle;
            });

        AddGameObject(particle1);
    }

    {
        auto particle2 = CreateGameObjectInstance(1.f, vec2(400, 555));

        Particle particle_2;
        particle_2.position      = particle2->worldTransform.GetPosition();
        particle_2.velocity      = vec3(0, 0.01, 0);
        particle_2.rotation      = 0;
        particle_2.scale         = 4;
        particle_2.gravityEffect = 0.0f;
        particle_2.lifeTime      = 2.6f;

        particle2->AddComponent<Components::ParticleEffectComponent>()
            .SetParticle(particle_2)
            .SetTexture("Textures/Particles/fire1_rows_8.png")
            .SetParticlesPerSec(100)
            .EnableAnimation()
            .SetSpeed(1.f)
            .SetBlendFunction(BlendFunctionType::SRC_ALPHA)
            .SetEmitFunction([](const Particle& referenceParticle) -> Particle {
                Particle particle = referenceParticle;

                float dirX        = Random() - 0.5f;
                float dirZ        = Random() - 0.5f;
                particle.velocity = vec3(dirX, 1, dirZ);

                float r = 2.f;
                particle.position += vec3(dirX, 0.f, dirZ) * r;

                float l = Random() / 2.f * particle.lifeTime + particle.lifeTime * 0.75f;

                particle.lifeTime = l;

                return particle;
            });

        AddGameObject(particle2);
    }

    CreateAndAddGameEntity("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/bottom2.obj", 10.f,
                           vec2(400, 570));
    CreateAndAddGameEntity("Meshes/Barrel/barrel.obj", 1.f, vec2(395, 565));
    CreateAndAddGameEntity("Meshes/sponza/sponza_mod.obj", 60.f, vec2(395, 665));
    CreateAndAddGameEntity("Meshes/Bialczyk/Bialczyk.obj", 30.f, vec2(395, 570));

    for (uint32_t x = 0; x < 4; x++)
    {
        CreateAndAddGameEntity("Meshes/Fern/fern.obj", 3.f, vec2(395, 560 - 5 * x), x);
    }

    {
        auto uplayer   = CreateGameObjectInstance(1.8f, vec2(395, 560), true);
        auto& animator = uplayer->AddComponent<Components::Animator>().SetAnimation("Idle");

        uplayer->AddComponent<Components::RendererComponent>().AddModel(
            "Meshes/DaeAnimationExample/CharacterMultiple.dae");
        player               = uplayer.get();
        characterController_ = std::make_shared<common::Controllers::CharacterController>(
            player->worldTransform, playerStats_.runSpeed, playerStats_.turnSpeed, playerStats_.jumpPower);
        playerInputController_ =
            std::make_shared<PlayerInputController>(&animator, inputManager_, characterController_.get());
        AddGameObject(uplayer);
    }

    // for (const auto& terrain : terrains_)
    {
        auto grass = CreateGameObjectInstance(1.8f, vec2(395, 560), true);

        auto grass_position = CreateGrassPositions(nullptr, vec2(375, 550));

        grass->AddComponent<Components::GrassRendererComponent>()
            .SetPositions(grass_position)
            .SetTexture("Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png");

        AddGameObject(grass);
    }

    dayNightCycle.SetDirectionalLight(&directionalLight);
    dayNightCycle.SetTime(.5f);

    camera = std::make_unique<FirstPersonCamera>(inputManager_, displayManager_);
    camera->SetPosition(player->worldTransform.GetPosition() + vec3(0, 1, 0));
    // SetCamera(std::make_unique<CThirdPersonCamera>(inputManager_, &player->worldTransform));
    camType = CameraType::FirstPerson;

    KeyOperations();

    return 0;
}

int MainScene::Update(float dt)
{
    deltaTime = dt;
    if (camera == nullptr)
    {
        Log("MainScene::Update camera is nullptr.");
        return -1;
    }

    //	renderersManager_->GuiTexture("shadowMap").texture->SetExistId(EngineConf.texturesIds["shadowMap"]);

    //   gloabalTime += deltaTime;
    //   timeClock += deltaTime;

    if (timeClock > 1.f)
    {
        timeClock                                   = 0;
        renderersManager_->GuiText("gameTime").text = "Game Time: " + std::to_string(dayNightCycle.GetCurrentHour().x) +
                                                      ":" + std::to_string(dayNightCycle.GetCurrentHour().y);
    }

    // dayNightCycle.Update(deltaTime);

    CheckCollisions(deltaTime);
    UpdatePlayerandCamera(deltaTime);

    renderersManager_->GuiText("playerPos").text = Utils::ToString(player->worldTransform.GetPosition());
    renderersManager_->GuiText("cameraPos").text = Utils::ToString(camera->GetPosition());
    return 0;
}

void MainScene::UpdatePlayerandCamera(float time)
{
    // camera->CalculateInput();
    characterController_->Update(deltaTime);
    // camera->Move();
}

void MainScene::KeyOperations()
{
    bool run = true;
    inputManager_->SubscribeOnKeyDown(KeyCodes::M, [&, run]() mutable {
        if (run)
        {
            player->GetComponent<Components::Animator>()->ChangeAnimation("Idle");
            run = false;
        }
        else
        {
            player->GetComponent<Components::Animator>()->ChangeAnimation("Run");
            run = true;
        }
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [&]() {
        // auto d = camera->GetDistance() - 0.5f;
        // camera->SetDistance(d);
    });

    inputManager_->SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [&]() {
        // auto d = camera->GetDistance() + 0.5f;
        // camera->SetDistance(d);
    });

    inputManager_->SubscribeOnKeyDown(KeyCodes::R, [&]() { renderersManager_->ReloadShaders(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::L, [&]() { renderersManager_->SwapLineFaceRender(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::T, [&]() { player->worldTransform.SetPosition(vec3(0, 0, 0)); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::C, [&]() {
        auto pos      = camera->GetPosition();
        auto rotation = camera->GetRotation();

        if (camType == CameraType::FirstPerson)
        {
            camType     = CameraType::ThridPerson;
            auto camera = std::make_unique<ThirdPersonCamera>(inputManager_, &player->worldTransform);
            SetCamera(std::move(camera));
        }
        else if (camType == CameraType::ThridPerson)
        {
            camType     = CameraType::FirstPerson;
            auto camera = std::make_unique<FirstPersonCamera>(inputManager_, displayManager_);
            camera->SetPosition(pos);
            camera->SetPitch(rotation.x);
            camera->SetYaw(rotation.y);
            camera->SetRoll(rotation.z);
            SetCamera(std::move(camera));
        }
    });
}

void MainScene::CheckCollisions(float dt)
{
    float g        = 9.8f * deltaTime;
    auto playerPos = player->worldTransform.GetPosition();

    playerPos.y -= g;

    bool wasCollision = false;


    if (!wasCollision)
        player->worldTransform.IncrasePosition(0.f, -g, 0.f);
}

void MainScene::AddTerrain(const TerrainTexturesFilesMap& textures, const glm::vec3& position)
{
    auto object = CreateGameObjectInstance(1.f, vec2(0));
    object->AddComponent<Components::TerrainRendererComponent>().LoadTextures(textures);

    auto& terrainShapeComponent = object->AddComponent<Components::TerrainShape>()
                                      .SetHeightMap(textures.at(TerrainTextureType::displacementMap));

    AddGameObject(object);
}

std::vector<float> MainScene::CreateGrassPositions(GameObject* object, vec2 pos)
{
    std::vector<float> grass_positions;
    for (float y = pos.y; y < pos.y + 50.f; y += .5f)
    {
        for (float x = pos.x; x < pos.x + 50.f; x += .5f)
        {
            float xpos = x + ((rand() % 400 - 200) / 10.f);
            float zpos = y + ((rand() % 400 - 200) / 10.f);
            float ypos = 0.f;

            if (object != nullptr)
            {
                auto height = object->CollisionDetection(glm::vec3(xpos, 0, zpos));
                if (height)
                    ypos = height.value().y;
            }
            grass_positions.push_back(xpos);
            grass_positions.push_back(ypos);
            grass_positions.push_back(zpos);
        }
    }
    return grass_positions;
}

std::unique_ptr<GameEngine::GameObject> MainScene::CreateGameObjectInstance(float scale, const vec2& position,
                                                                            bool isDynamic)
{
    auto obj = CreateGameObject();
    obj->worldTransform.SetScale(scale);
    obj->worldTransform.isDynamic_ = isDynamic;
    vec3 obj_pos(position.x, 0, position.y);

    obj->worldTransform.SetPosition(obj_pos);
    obj->worldTransform.TakeSnapShoot();
    return obj;
}

void MainScene::CreateAndAddGameEntity(const std::string& filename, float scale, const vec2& position,
                                       uint32_t textureIndex, bool isDynamic)
{
    auto object = CreateGameObjectInstance(scale, position, isDynamic);
    object->AddComponent<Components::RendererComponent>().AddModel(filename).SetTextureIndex(textureIndex);
    AddGameObject(object);
}

void MainScene::InitGui()
{
}
