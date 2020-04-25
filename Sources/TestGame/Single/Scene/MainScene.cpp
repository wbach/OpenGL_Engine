
#include "MainScene.h"
#include "GLM/GLMUtils.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Physics/BoxShape.h"
#include "GameEngine/Components/Physics/MeshShape.h"
#include "GameEngine/Components/Physics/Rigidbody.h"
#include "GameEngine/Components/Physics/SphereShape.h"
#include "GameEngine/Components/Physics/Terrain/TerrainShape.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Components/Renderer/Particles/ParticleEffectComponent.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Components/Renderer/Skydome/SkydomeComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainMeshRendererComponent.h"
#include "GameEngine/Components/Renderer/Terrain/TerrainRendererComponent.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Components/Renderer/Water/WaterRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Lights/Light.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Resources/Textures/HeightMap.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "GraphicsApi/BlendFunctionsTypes.h"
#include "Renderers/GUI/Button/GuiButton.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "Renderers/GUI/Window/GuiWindow.h"
#include "SingleTon.h"
#include "Thread.hpp"

using namespace GameEngine;

const std::string sceneFile = "partScene.xml";
const std::string GUI_FILE  = "gui.xml";

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
        {TerrainTextureType::heightmap, "Textures/Terrain/HeightMaps/ExportedTerrain.terrain"}
    };
    // clang-format on
}

TerrainTexturesFilesMap CreateOreonTerrainTexturesMap()
{
    // clang-format off
    return
    {
        {TerrainTextureType::blendMap, "Textures/Terrain/BlendMaps/testBlendMap.png"},
        {TerrainTextureType::backgorundTexture, "Textures/Terrain/Ground/oreon/grass0_DIF.jpg"},
        {TerrainTextureType::backgorundTextureNormal, "Textures/Terrain/Ground/oreon/grass0_NRM.jpg"},
        {TerrainTextureType::redTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        {TerrainTextureType::greenTexture, "Textures/Terrain/Ground/grassFlowers.png"},
        {TerrainTextureType::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png"},
        {TerrainTextureType::heightmap, "Textures/Terrain/HeightMaps/ExportedTerrain.terrain"}
    };
    // clang-format on
}

MainScene::MainScene()
    : Scene("MainScene")
{
}

MainScene::~MainScene()
{
    DEBUG_LOG(__FUNCTION__);
}

float Random()
{
    return static_cast<float>(rand() % 100000) / 100000.f;
}

int MainScene::Initialize()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::ESCAPE, [&]() { addEngineEvent(EngineEvent::QUIT); });

    DEBUG_LOG("MainScene::Initialize()");
    resourceManager_->GetTextureLaoder().SetHeightMapFactor(10.f);
    resourceManager_->GetTextureLaoder().CreateHeightMap("Textures/Terrain/HeightMaps/World.png",
                                                         "Textures/Terrain/HeightMaps/output.terrain", vec3(1.f));

    InitGui();

    /* renderersManager_->GuiText("gameTime").position  = vec2(0.75, 0.9);
     renderersManager_->GuiText("gameTime").m_size    = 0.5f;
     renderersManager_->GuiText("gameTime").text = "Game Time" + std::to_string(dayNightCycle.GetCurrentHour().x) + ":"
     + std::to_string(dayNightCycle.GetCurrentHour().y); renderersManager_->GuiText("cameraPos").position = vec2(-0.9,
     -0.8); renderersManager_->GuiText("cameraPos").m_size   = .5f; renderersManager_->GuiText("cameraPos").colour   =
     vec3(.8f, 0.f, 0.f);

     GameEngine::Renderer::Gui::GuiTextureElement guiTexture;
     guiTexture.texture = resourceManager_->GetTextureLaoder().LoadTexture("GUI/Package1/Some-Box.png", false);
     guiTexture.SetPosition(vec2(0.5, 0.5));
     guiTexture.SetScale(vec2(0.3));

     auto windowsSize = renderersManager_->GetProjection().GetWindowSize();


     auto fontId      = resourceManager_->GetGraphicsApi().GetWindowApi()->OpenFont(fontPath, fontSize);


     {
         auto fontImage = resourceManager_->GetGraphicsApi().GetWindowApi()->RenderFont(fontId, "example font text.",
                                                                                        vec4(0.5, 0.5, 0.5, 1.f), 0);

         auto fontTexture = resourceManager_->GetTextureLaoder().CreateTexture(
             "FontImage_" + std::to_string(fontImage.id), GraphicsApi::TextureType::U8_RGBA,
             GraphicsApi::TextureFilter::NEAREST, GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE,
             fontImage.size, fontImage.pixels);

         auto scale = vec2(fontImage.size.x, fontImage.size.y);
         scale.x    = scale.x * 1.f / windowsSize.x;
         scale.y    = scale.y * 1.f / windowsSize.y;

         renderersManager_->GuiTexture("fontTexture").texture = fontTexture;
         renderersManager_->GuiTexture("fontTexture").SetPosition(vec2(0.5, 0.5));
         renderersManager_->GuiTexture("fontTexture").SetColor(vec3(1, 1, 1));
         renderersManager_->GuiTexture("fontTexture").SetScale(scale);
     }*/

    //{
    //    auto fontImage = resourceManager_->GetGraphicsApi().GetWindowApi()->RenderFont(fontId, "example font text.",
    //        vec4(0.5, 0.5, 0.5, 1.f), 0);

    //    auto fontTexture = resourceManager_->GetTextureLaoder().CreateTexture(
    //        "FontImage_" + std::to_string(fontImage.id), GraphicsApi::TextureType::U8_RGBA,
    //        GraphicsApi::TextureFilter::NEAREST, GraphicsApi::TextureMipmap::NONE, GraphicsApi::BufferAtachment::NONE,
    //        fontImage.size, fontImage.pixels);

    //    auto scale = vec2(fontImage.size.x, fontImage.size.y);
    //    scale.x = scale.x * 1.f / windowsSize.x;
    //    scale.y = scale.y * 1.f / windowsSize.y;

    //    renderersManager_->GuiTexture("fontTexture_shadow").texture = fontTexture;
    //    renderersManager_->GuiTexture("fontTexture_shadow").SetPosition(vec2(0.50, 0.5));
    //    renderersManager_->GuiTexture("fontTexture_shadow").SetColor(vec3(0.0));
    //    renderersManager_->GuiTexture("fontTexture_shadow").SetScale(scale);
    //}

    // renderersManager_->GuiTexture("fontBackground") = guiTexture;

    RegisterParticleEmitFunction("water", [](const Particle& referenceParticle) -> Particle {
        Particle particle = referenceParticle;

        float dirX        = Random() - 0.5f;
        float dirZ        = Random() - 0.5f;
        particle.velocity = vec3(dirX, 1, dirZ);

        return particle;
    });

    RegisterParticleEmitFunction("fire", [](const Particle& referenceParticle) -> Particle {
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

    // CreateExmapleStrtupObject();
    LoadFromFile(sceneFile);

    dayNightCycle.SetDirectionalLight(&directionalLight);
    dayNightCycle.SetTime(.5f);

    auto skydome = CreateGameObject();
    skydome->AddComponent<Components::SkydomeComponent>();
    AddGameObject(std::move(skydome));

    centerObjectPosition_ = vec3(0);
    auto geralt           = CreateGameObjectInstance("Geralt", 1.8f, vec2(0), false);
    geralt->GetTransform().SetPosition(centerObjectPosition_);
    geralt->AddComponent<Components::RendererComponent>().AddModel("Meshes/Geralt/geralt.obj");
    AddGameObject(std::move(geralt));

    auto water = CreateGameObject("Water");
    water->AddComponent<Components::WaterRendererComponent>()
        .LoadTextures("Textures/Water/waterDUDV.png", "Textures/Water/waternormal.png")
        .SetWaveSpeed(10.f);
    AddGameObject(std::move(water));

    SetDirectionalLightColor(vec3(0.4));
    pointLight_ = &AddLight(Light(vec3(2, 4, 2), vec3(1), vec3(0, 0, 0.1)));

    {
        auto obj   = CreateGameObjectInstance("LightBulb", 0.5f, vec2(0, 0), true);
        lightBulb_ = obj.get();
        obj->AddComponent<Components::RendererComponent>().AddModel("Meshes/BulbPack/Bulb.obj");
        AddGameObject(std::move(obj));
    }

    {
        auto obj = CreateGameObjectInstance("Barrel", 1.8f, vec2(-2, 0), true);
        obj->AddComponent<Components::RendererComponent>().AddModel("Meshes/Barrel/barrel.obj");
        AddGameObject(std::move(obj));
    }

    {
        auto obj = CreateGameObjectInstance("Garen", 1.8f, vec2(-4, 0), true);
        obj->AddComponent<Components::RendererComponent>().AddModel("Meshes/Garen/garen_idle.fbx");
        AddGameObject(std::move(obj));
    }

    {
        auto uplayer = CreateGameObjectInstance("Player", 1.8f, vec2(2, 0), true);
        uplayer->AddComponent<Components::RendererComponent>().AddModel(
            "Meshes/DaeAnimationExample/CharacterMultiple.dae");

        auto& animator = uplayer->AddComponent<Components::Animator>().SetAnimation("Idle");

        player = uplayer.get();

        characterController_ = std::make_shared<common::Controllers::CharacterController>(
            player->GetTransform(), playerStats_.runSpeed, playerStats_.turnSpeed, playerStats_.jumpPower);

        playerInputController_ =
            std::make_shared<PlayerInputController>(&animator, inputManager_, characterController_.get());
        AddGameObject(std::move(uplayer));
    }
    camera_ = std::make_unique<FirstPersonCamera>(*inputManager_, *displayManager_);
    camera.Set(*camera_);
    camera.SetPosition(vec3(.5, 3.5, 2.1));
    // camera.SetPosition(vec3(-5107.217, 3324.774, 5352.738));
    camera.LookAt(vec3(0, 2, 0));
    // SetCamera(std::make_unique<CThirdPersonCamera>(inputManager_, &player->worldTransform));
    camType = CameraType::FirstPerson;

    KeyOperations();

    return 0;
}

template <typename Shape>
void MainScene::AddPhysicObject(const std::string& modelFilename, const vec3& pos, const vec3& shapePositionOffset,
                                const vec3& dir, float scale, bool isStatic)
{
    auto object = CreateGameObject(Utils::GetFilename(modelFilename));
    object->GetTransform().SetPosition(pos);
    object->GetTransform().SetScale(scale);
    object->AddComponent<Components::RendererComponent>().AddModel(modelFilename);

    auto& shape = object->AddComponent<Shape>().SetSize(scale);
    shape.SetPostionOffset(shapePositionOffset);
    object->AddComponent<Components::Rigidbody>()
        .SetIsStatic(isStatic)
        .SetCollisionShape(shape.GetType())
        .SetVelocity(dir);

    AddGameObject(std::move(object));
}

int MainScene::Update(float dt)
{
    deltaTime = dt;

    vec3 lightPos = pointLight_->GetPosition();
    lightPos      = Utils::RotateObject(centerObjectPosition_, lightPos, ToRadians(10.f * dt));
    pointLight_->SetPosition(lightPos);

    lightBulb_->GetTransform().SetPosition(pointLight_->GetPosition());

    //	renderersManager_->GuiTexture("shadowMap").texture->SetExistId(EngineConf.texturesIds["shadowMap"]);

    //   gloabalTime += deltaTime;
    timeClock += deltaTime;

    if (timeClock > 0.25f)
    {
        timeClock = 0;
        // renderersManager_->GuiText("gameTime").text = "Game Time: " +
        // std::to_string(dayNightCycle.GetCurrentHour().x) +
        //":" + std::to_string(dayNightCycle.GetCurrentHour().y);

        guiManager_->Get<GuiTextElement>("playerPos")
            ->SetText("World Player position : " + std::to_string(player->GetWorldTransform().GetPosition()));
        guiManager_->Get<GuiTextElement>("rendererFps")
            ->SetText("Render thread fps : " + std::to_string((int)displayManager_->GetTime().fps));

        guiElementFactory_->ReadGuiFile(GUI_FILE);
    }

    // dayNightCycle.Update(deltaTime);

    CheckCollisions(deltaTime);
    UpdatePlayerandCamera(deltaTime);

    // renderersManager_->GuiText("cameraPos").text = std::to_string(camera.GetPosition());
    return 0;
}

void MainScene::UpdatePlayerandCamera(float time)
{
    // camera.CalculateInput();
    characterController_->Update(deltaTime);
    if (terrainHeightGetter_)
    {
        auto height = terrainHeightGetter_->GetHeightofTerrain(characterController_->GetTransform().GetPositionXZ());
        if (height)
        {
            characterController_->GetTransform().SetYPosition(*height);
            characterController_->GetTransform().TakeSnapShoot();
        }
    }

    // camera.Move();
}

void MainScene::KeyOperations()
{
    inputManager_->SubscribeOnKeyDown(KeyCodes::F, [&]() {
        auto dir = GetCamera().GetDirection();
        dir      = glm::normalize(dir);
        auto pos = GetCamera().GetPosition();
        AddPhysicObject<Components::SphereShape>("Meshes/sphere.obj", pos + dir, vec3(0), dir * 20.f, 1.f, false);
        DEBUG_LOG("Dir : " + std::to_string(dir) + ", Pos : " + std::to_string(pos) +
                  ", Objecsts : " + std::to_string(GetAllGameObjectsPtrs().size()));
    });

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

    if (inputManager_->GetKey(KeyCodes::LCTRL))
    {
        inputManager_->ShowCursor(true);
        return;
    }

    inputManager_->SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [&]() {
        // auto d = camera.GetDistance() - 0.5f;
        // camera.SetDistance(d);
    });

    inputManager_->SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [&]() {
        // auto d = camera.GetDistance() + 0.5f;
        // camera.SetDistance(d);
    });

    inputManager_->SubscribeOnKeyUp(KeyCodes::G, [&]() { guiElementFactory_->ReadGuiFile(GUI_FILE); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::F1, [&]() { SaveToFile(sceneFile); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::P, [this]() { renderersManager_->GetDebugRenderer().Enable(); });
    inputManager_->SubscribeOnKeyDown(KeyCodes::O, [this]() { renderersManager_->GetDebugRenderer().Disable(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::R, [&]() { renderersManager_->ReloadShaders(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::L, [&]() { renderersManager_->SwapLineFaceRender(); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::T, [&]() { player->GetTransform().SetPosition(vec3(0, 0, 0)); });

    inputManager_->SubscribeOnKeyDown(KeyCodes::C, [&]() {
        auto pos      = camera.GetPosition();
        auto rotation = camera.GetRotation();

        if (camType == CameraType::FirstPerson)
        {
            camType = CameraType::ThridPerson;
            camera_ = std::make_unique<ThirdPersonCamera>(*inputManager_, player->GetTransform());
            camera.Set(*camera_);
        }
        else if (camType == CameraType::ThridPerson)
        {
            camType = CameraType::FirstPerson;
            camera_ = std::make_unique<FirstPersonCamera>(*inputManager_, *displayManager_);
            camera_->SetPosition(pos);
            camera_->SetRotation(rotation);
            camera.Set(*camera_);
        }
    });
}

void MainScene::CheckCollisions(float)
{
    return;

    float g        = 9.8f * deltaTime;
    auto playerPos = player->GetTransform().GetPosition();

    playerPos.y -= g;

    bool wasCollision = false;

    if (!wasCollision)
        player->GetTransform().IncrasePosition(0.f, -g, 0.f);
}

void MainScene::AddTerrain(const TerrainTexturesFilesMap& textures, const glm::vec3& position)
{
    auto object = CreateGameObjectInstance("Terrain", 1.f, vec2(0));
    resourceManager_->GetTextureLaoder().SetHeightMapFactor(10.f);
    object->AddComponent<Components::TerrainMeshRendererComponent>().LoadTextures(textures);

    auto& terrainShapeComponent =
        object->AddComponent<Components::TerrainShape>().SetHeightMap(textures.at(TerrainTextureType::heightmap));

    auto rigidbody = object->AddComponent<Components::Rigidbody>()
                         .SetCollisionShape(terrainShapeComponent.GetType())
                         .SetIsStatic(true);

    auto image = terrainShapeComponent.GetHeightMap()->GetImage();

    AddGameObject(std::move(object));
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

            if (terrainHeightGetter_)
            {
                auto height = terrainHeightGetter_->GetHeightofTerrain(xpos, zpos);
                if (height)
                {
                    ypos = *height;
                }
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
    vec3 obj_pos(position.x, 0, position.y);
    obj->GetTransform().SetScale(scale);
    obj->GetTransform().SetPosition(obj_pos);
    return obj;
}

std::unique_ptr<GameEngine::GameObject> MainScene::CreateGameObjectInstance(const std::string& name, float scale,
                                                                            const vec2& position, bool isDynamic)
{
    auto obj = CreateGameObject(name);
    vec3 obj_pos(position.x, 0, position.y);
    obj->GetTransform().SetScale(scale);
    obj->GetTransform().SetPosition(obj_pos);
    return obj;
}

void MainScene::CreateAndAddGameEntity(const std::string& filename, float scale, const vec2& position,
                                       uint32_t textureIndex, bool isDynamic)
{
    auto object = CreateGameObjectInstance(Utils::GetFilename(filename), scale, position, isDynamic);

    if (terrainHeightGetter_)
    {
        auto height = terrainHeightGetter_->GetHeightofTerrain(position);
        if (height)
        {
            object->GetTransform().SetYPosition(*height);
        }
    }

    object->AddComponent<Components::RendererComponent>().AddModel(filename).SetTextureIndex(textureIndex);
    AddGameObject(std::move(object));
}

void MainScene::CreateExmapleStrtupObject()
{
    // clang-format off
    {
        std::array<std::string, 6> nightTextures{ "Skybox/Night/right.png", "Skybox/Night/left.png",
                                                "Skybox/Night/top.png",   "Skybox/Night/bottom.png",
                                                "Skybox/Night/back.png",  "Skybox/Night/front.png" };

        std::array<std::string, 6> dayTextures{ "Skybox/TropicalSunnyDay/right.png", "Skybox/TropicalSunnyDay/left.png",
                                                "Skybox/TropicalSunnyDay/top.png",   "Skybox/TropicalSunnyDay/bottom.png",
                                                "Skybox/TropicalSunnyDay/back.png",  "Skybox/TropicalSunnyDay/front.png" };

        auto skybox = CreateGameObjectInstance("skybox", 1.f, vec2(0));
        skybox->AddComponent<Components::SkyBoxComponent>()
            .SetModel("Meshes/SkyBox/cube.obj")
            .SetNightTexture(nightTextures)
            .SetDayTexture(dayTextures);

        AddGameObject(std::move(skybox));
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
                treePos                       = treePos + vec3(-45, 0, -100);
                treePositions[x + size.x * y] = treePos;
            }
        }
        auto tree1 = CreateGameObjectInstance("trees", 20.f, vec2(0, 0));
        tree1->AddComponent<Components::TreeRendererComponent>()
            .SetPositions(treePositions, size)
            .SetTopModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/top.obj")
            .SetBottomModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/bottom2T.obj");
        AddGameObject(std::move(tree1));
    }

    {
        auto particle1 = CreateGameObjectInstance("particle1", 1.f, vec2(5, 10));

        Particle particle;
        particle.position      = particle1->GetTransform().GetPosition();
        particle.velocity      = vec3(0, 0.1, 0);
        particle.rotation      = 0;
        particle.scale         = 8;
        particle.gravityEffect = true;
        particle.lifeTime      = 2.f;

        particle1->AddComponent<Components::ParticleEffectComponent>()
            .SetParticle(particle)
            .SetTexture("Textures/Particles/water.png")
            .SetParticlesPerSec(10)
            .SetBlendFunction(GraphicsApi::BlendFunctionType::ONE)
            .SetEmitFunction("water", [](const Particle& referenceParticle) -> Particle {
                Particle particle = referenceParticle;

                float dirX        = Random() - 0.5f;
                float dirZ        = Random() - 0.5f;
                particle.velocity = vec3(dirX, 1, dirZ);

                return particle;
            });

        AddGameObject(std::move(particle1));
    }

    {
        auto particle2 = CreateGameObjectInstance("particle2", 1.f, vec2(5, 5));

        Particle particle_2;
        particle_2.position      = particle2->GetTransform().GetPosition();
        particle_2.velocity      = vec3(0, 0.01, 0);
        particle_2.rotation      = 0;
        particle_2.scale         = 4;
        particle_2.gravityEffect = false;
        particle_2.lifeTime      = 2.6f;

        particle2->AddComponent<Components::ParticleEffectComponent>()
            .SetParticle(particle_2)
            .SetTexture("Textures/Particles/fire1_rows_8.png")
            .SetParticlesPerSec(100)
            .EnableAnimation()
            .SetSpeed(1.f)
            .SetBlendFunction(GraphicsApi::BlendFunctionType::SRC_ALPHA)
            .SetEmitFunction("fire", [](const Particle& referenceParticle) -> Particle {
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

        AddGameObject(std::move(particle2));
    }

    CreateAndAddGameEntity("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/bottom2.obj", 10.f,
                           vec2(400, 570));
    AddPhysicObject<Components::BoxShape>("Meshes/Barrel/barrel.obj", vec3(0, 0, 15), vec3(0, -.5f, 0), vec3(0), 1.f,
                                          true);
    AddPhysicObject<Components::BoxShape>("Meshes/Bialczyk/bialczyk_dom.obj", vec3(-15, 0, 10), vec3(0, -1.f, 0),
                                          vec3(0), 20.f, true);
    AddPhysicObject<Components::BoxShape>("Meshes/Bialczyk/bialczyk_stajnia.obj", vec3(15, 0, 15), vec3(0, -1.f, 0),
                                          vec3(0), 20.f, true);
    AddPhysicObject<Components::BoxShape>("Meshes/Bialczyk/well.obj", vec3(2, 0, 15), vec3(0, -.5f, 0), vec3(0), 2.f,
                                          true);

    for (uint32_t x = 0; x < 4; x++)
    {
        CreateAndAddGameEntity("Meshes/Fern/fern.obj", 3.f, vec2(0, 10 - 5 * x), x);
    }
    {
        auto grass = CreateGameObjectInstance("Grass", 1.8f, vec2(0, 10), true);

        auto grass_position = CreateGrassPositions(nullptr, vec2(0, 0));

        grass->AddComponent<Components::GrassRendererComponent>()
            .SetPositions(grass_position)
            .SetTexture("Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png");

        AddGameObject(std::move(grass));
    }
}

void MainScene::InitGui()
{
    guiElementFactory_->ReadGuiFile(GUI_FILE);
}
