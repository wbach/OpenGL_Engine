#include "MainScene.h"
#include "SingleTon.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "GameEngine/Engine/Engine.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "GameEngine/Camera/FirstPersonCamera.h"
#include "GameEngine/Camera/ThridPersonCamera.h"
#include "GameEngine/Renderers/GUI/GuiRenderer.h"
#include "GameEngine/Renderers/GUI/Text/GuiText.h"
#include "GameEngine/Resources/Textures/Image.h"
#include "GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "Renderers/GUI/Texutre/GuiTextureElement.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GLM/GLMUtils.h"
#include "Thread.hpp"

MainScene::MainScene()
    : GameEngine::Scene("MainScene")
{	
}

MainScene::~MainScene()
{
	Log(__FUNCTION__);
}

int MainScene::Initialize()
{
	Log("MainScene::Initialize()");
	
	renderersManager_->GuiText("playerPos").position = vec2(-0.9, -0.9);
	renderersManager_->GuiText("playerPos").m_size = .5f;
	renderersManager_->GuiText("gameTime").position = vec2(0.75, 0.9);
	renderersManager_->GuiText("gameTime").m_size = 0.5f;
	renderersManager_->GuiText("gameTime").text = "Game Time" + std::to_string(dayNightCycle.GetCurrentHour().x) + ":" + std::to_string(dayNightCycle.GetCurrentHour().y);

	GameEngine::Renderer::Gui::GuiTextureElement guiTexture;
	guiTexture.texture = new CTexture();
	guiTexture.SetScale(vec2(0.25, 0.25));
	guiTexture.SetPosition(vec2(0.5, 0.5));
	renderersManager_->GuiTexture("shadowMap") = guiTexture;

	auto terrain_textures = CreateTerrainTexturesMap();
	AddTerrain(terrain_textures, glm::vec3(1));

	AddEntity("Meshes/Bialczyk/Bialczyk.obj", 30.f, vec2(395, 570));
	AddEntity("Meshes/Barrel/barrel.obj", 1.f, vec2(395, 565));
	
	player = AddEntity("Meshes/DaeAnimationExample/CharacterRunningSmooth.dae", 1.8f, vec2(395, 560), true);
	player->AddComponent(std::move(componentFactory_.Create(GameEngine::Components::ComponentsType::Animator)));
	auto animator = player->GetComponent<GameEngine::Components::Animator>(GameEngine::Components::ComponentsType::Animator);
	auto eplayer = static_cast<CEntity*>(player);
	animator->animationClips_ = eplayer->GetModel()->animationClips_;
	animator->SetSkeleton(&eplayer->GetModel()->skeleton_);
	animator->animationSpeed_ = 0.5f;

	for (const auto& terrain : terrains)
	{
		auto grass_position = CreateGrassPositions(terrain, vec2(375, 550));

		auto grass_obj = ObjectBuilder::CreateGrass(resourceManager, grass_position, "Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png");
		AddGameObject(grass_obj);
		renderersManager_->Subscribe(grass_obj);
	}

	characterController_ = std::make_shared<common::Controllers::CharacterController>(player->worldTransform, playerStats_.runSpeed, playerStats_.turnSpeed, playerStats_.jumpPower);
	playerInputController_ = std::make_shared<PlayerInputController>(inputManager_, characterController_.get());
  
    dayNightCycle.SetDirectionalLight(&directionalLight);
    dayNightCycle.SetTime(.5f);

    camera = std::make_unique<CFirstPersonCamera>(inputManager_, displayManager_);
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

	renderersManager_->GuiTexture("shadowMap").texture->SetExistId(EngineConf.texturesIds["shadowMap"]);

 //   gloabalTime += deltaTime;
 //   timeClock += deltaTime;

    if(timeClock > 1.f)
    {
        timeClock = 0;
		renderersManager_->GuiText("gameTime").text = "Game Time: " + std::to_string(dayNightCycle.GetCurrentHour().x) + ":" + std::to_string(dayNightCycle.GetCurrentHour().y);
    }

    //dayNightCycle.Update(deltaTime);

    CheckCollisions(deltaTime);
	UpdatePlayerandCamera(deltaTime);

	renderersManager_->GuiText("playerPos").text = Utils::ToString(player->worldTransform.GetPosition());
    return 0;
}

void MainScene::UpdatePlayerandCamera(float time)
{	
	//camera->CalculateInput();
	characterController_->Update(deltaTime);
	//camera->Move();
}

void MainScene::KeyOperations()
{
	inputManager_->SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [&]()
	{
		auto d = camera->GetDistance() - 0.5f;
		camera->SetDistance(d);
	});

	inputManager_->SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [&]()
	{
		auto d = camera->GetDistance() + 0.5f;
		camera->SetDistance(d);
	});

	inputManager_->SubscribeOnKeyDown(KeyCodes::R, [&]()
	{
		renderersManager_->ReloadShaders();
	});

	inputManager_->SubscribeOnKeyDown(KeyCodes::L, [&]()
	{
		renderersManager_->SwapLineFaceRender();
	});

	inputManager_->SubscribeOnKeyDown(KeyCodes::T, [&]()
	{
		player->worldTransform.SetPosition(vec3(0, 0, 0));
	});

	inputManager_->SubscribeOnKeyDown(KeyCodes::C, [&]()
	{
		auto pos = camera->GetPosition();
		auto rotation = camera->GetRotation();

		if (camType == CameraType::FirstPerson)
		{
			camType = CameraType::ThridPerson;
			SetCamera(std::make_unique<CThirdPersonCamera>(inputManager_, &player->worldTransform));
		}
		else if (camType == CameraType::ThridPerson)
		{
			camType = CameraType::FirstPerson;
			SetCamera(std::make_unique<CFirstPersonCamera>(inputManager_, displayManager_));
		}
		camera->SetPosition(pos);
		camera->SetPitch(rotation.x);
		camera->SetYaw(rotation.y);
		camera->SetRoll(rotation.z);
	});
}

void MainScene::CheckCollisions(float dt)
{
	float g = 9.8f * deltaTime;
	auto playerPos = player->worldTransform.GetPosition();

	playerPos.y -= g;

	bool wasCollision = false;

	for (auto& terrain : terrains)
	{
        auto new_position = terrain->CollisionDetection(playerPos);

		if (!new_position)
			continue;

		if ( playerPos.y < new_position.value().y)
		{
			wasCollision = true;
			player->worldTransform.SetPosition(new_position.value());
			break;
		}
	}

	if (!wasCollision)	player->worldTransform.IncrasePosition(0.f, -g, 0.f);
}

TerrainTexturesMap MainScene::CreateTerrainTexturesMap()
{
    return
    {
        { Terrain::blendMap , "Textures/Terrain/BlendMaps/testBlendMap.png"},
        { Terrain::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
        { Terrain::redTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png",  },
		{ Terrain::rockTexture, "Textures/Terrain/Ground/G3_Nature_Wall_Stone_12_Diffuse_01.png", },
		{ Terrain::snowTexture, "Textures/Terrain/Ground/snow512.png", },
        { Terrain::greenTexture,"Textures/Terrain/Ground/grassFlowers.png"},
        { Terrain::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
        { Terrain::displacementMap, "Textures/Terrain/HeightMaps/Terrain.terrain" }
    };
}

void MainScene::AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position)
{
	//, CreateGrassPositions(), "Data/Textures/G3_Nature_Plant_Grass_06_Diffuse_01.png")
    auto terrain = ObjectBuilder::CreateTerrain(resourceManager, textures);
	if (terrain == nullptr)
	{
		Error("MainScene::AddTerrain : terrain is nullptr.");
		return;
    }    
    AddGameObject(terrain, position);
    terrains.push_back(terrain);
    renderersManager_->Subscribe(terrain);
	terrains_.push_back(terrain);

	//for (int y = -10; y < 10; y++)
	//	for (int x = -10; x < 10; x++)
	//	{
	//		auto obj = ObjectBuilder::CreateEntity(&resourceManager, glm::vec3(0, 1.f, 0), "Meshes/Cube.obj");
	//		
	//		auto pos = terrain->CollisionDetection(vec3(x, 0.f, y));

	//		if (!pos)
	//			continue;

	//		AddGameObject(obj, pos.constValue());
	//		renderersManager_->Subscribe(obj);
	//	}
}

std::vector<float> MainScene::CreateGrassPositions(CGameObject* object, vec2 pos)
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

CGameObject* MainScene::AddEntity(const std::string& modelName, float scale, const vec2& position, bool isDynamic)
{
	auto obj = ObjectBuilder::CreateEntity(&resourceManager, modelName);
	obj->worldTransform.SetScale(scale);
	obj->worldTransform.isDynamic_ = isDynamic;

	vec3 obj_pos(position.x, 0, position.y);

	for (const auto& t : terrains_)
	{
		auto pos = t->CollisionDetection(obj_pos);
		if (pos)
		{
			obj_pos = pos.constValue();
			break;
		}
	}
	AddGameObject(obj, obj_pos);
	renderersManager_->Subscribe(obj);
	obj->worldTransform.TakeSnapShoot();
	return obj;
}

void MainScene::InitGui()
{

}
