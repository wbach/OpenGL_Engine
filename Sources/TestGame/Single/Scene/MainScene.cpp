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
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "GameEngine/Components/Renderer/TreeRendererComponent.h"
#include "GameEngine/Components/Renderer/ParticleEffectComponent.h"
#include "GameEngine/Api/BlendFunctionsTypes.h"
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

float Random()
{
	return static_cast<float>(rand() % 100000) / 100000.f;
}

int MainScene::Initialize()
{
	Log("MainScene::Initialize()");
	
	renderersManager_->GuiText("playerPos").position = vec2(-0.9, -0.9);
	renderersManager_->GuiText("playerPos").m_size = .5f;
	renderersManager_->GuiText("playerPos").colour = vec3(.8f);
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

	auto tree1 = AddGameObjectInstance(20.f, vec2(0, 0));
	auto treeComp = AddComponent<GameEngine::Components::TreeRendererComponent>(tree1);

	std::vector<vec3> treePositions;
	vec2ui size(10, 10);
	treePositions.resize(size.x * size.y);
	
	for(uint32 y = 0; y < size.y; y++)
		for (uint32 x = 0; x < size.x; x++)
		{
			vec3 treePos( 10.f * x, 0.f,10.f * y);
			treePos.x += static_cast<float>(rand() % 100) / 10.f;
			treePos.z += static_cast<float>(rand() % 100) / 10.f;
			treePos = treePos + vec3(350, 0, 450);

			for (auto& terrain : terrains)
			{
				auto new_position = terrain->CollisionDetection(treePos);
				if (!new_position)
					continue;

				treePos.y = new_position.constValue().y - .5f;
			}
			treePositions[x + size.x * y] = treePos;
		}
	treeComp->SetPositions(treePositions, size);
	treeComp->SetTopModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/top.obj");
	treeComp->SetBottomModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/bottom2T.obj");

	{
		auto particle1 = AddGameObjectInstance(1.f, vec2(400, 560));
		auto effect1 = AddComponent<GameEngine::Components::ParticleEffectComponent>(particle1);
		effect1->SetTexture("Textures/Particles/water.png");
		effect1->SetParticlesPerSec(10);
		effect1->SetBlendFunction(GameEngine::BlendFunctionType::ONE);
		effect1->SetEmitFunction([](const GameEngine::Particle& referenceParticle) -> GameEngine::Particle
		{
			GameEngine::Particle particle = referenceParticle;

			float dirX = Random() - 0.5f;
			float dirZ = Random() - 0.5f;
			particle.velocity = vec3(dirX, 1, dirZ);

			return particle;
		});


		GameEngine::Particle particle;
		particle.position = particle1->worldTransform.GetPosition();
		particle.velocity = vec3(0, 0.1, 0);
		particle.rotation = 0;
		particle.scale = 8;
		particle.gravityEffect = 1.f;
		particle.lifeTime = 2.f;
		effect1->SetParticle(particle);
	}

	{
		auto particle2 = AddGameObjectInstance(1.f, vec2(400, 555));
		auto effect2 = AddComponent<GameEngine::Components::ParticleEffectComponent>(particle2);
		effect2->SetTexture("Textures/Particles/fire1_rows_8.png");
		effect2->SetParticlesPerSec(100);
		effect2->UseAnimation();
		effect2->SetSpeed(1.f);
		effect2->SetBlendFunction(GameEngine::BlendFunctionType::SRC_ALPHA);
		effect2->SetEmitFunction([](const GameEngine::Particle& referenceParticle) -> GameEngine::Particle
		{
			GameEngine::Particle particle = referenceParticle;


			float dirX = Random() - 0.5f;
			float dirZ = Random() - 0.5f;
			particle.velocity = vec3(dirX, 1, dirZ);

			float r = 2.f;
			particle.position += vec3(dirX, 0.f, dirZ) * r;


			float l = Random() / 2.f * particle.lifeTime + particle.lifeTime * 0.75f;

			particle.lifeTime = l;

			return particle;
		});

		GameEngine::Particle particle_2;
		particle_2.position = particle2->worldTransform.GetPosition();
		particle_2.velocity = vec3(0, 0.01, 0);
		particle_2.rotation = 0;
		particle_2.scale = 4;
		particle_2.gravityEffect = 0.0f;
		particle_2.lifeTime = 2.6f;
		effect2->SetParticle(particle_2);

	}

	auto treeGo1 = AddGameObjectInstance(10.f, vec2(400, 570));
	AddComponent<GameEngine::Components::RendererComponent>(treeGo1)->AddModel("Meshes/woodland_pack_1/WOODLAND_PACK/WOODLAND_TREES/f_tree1/bottom2.obj");

	auto barrel = AddGameObjectInstance(1.f, vec2(395, 565));
	AddComponent<GameEngine::Components::RendererComponent>(barrel)->AddModel("Meshes/Barrel/barrel.obj");

	for (int x = 0; x < 4; x++)
	{
		auto fern = AddGameObjectInstance(3.f, vec2(395, 560 - 5 * x));
		auto fernRComp = AddComponent<GameEngine::Components::RendererComponent>(fern);
		fernRComp->AddModel("Meshes/Fern/fern.obj");
		fernRComp->textureIndex = x;
	}


	auto bialczyk  = AddGameObjectInstance(30.f, vec2(395, 570));
	AddComponent<GameEngine::Components::RendererComponent>(bialczyk)->AddModel("Meshes/Bialczyk/Bialczyk.obj");
	player = AddGameObjectInstance(1.8f, vec2(395, 560), true);
	auto animator = AddComponent<GameEngine::Components::Animator>(player);
	AddComponent<GameEngine::Components::RendererComponent>(player)->AddModel("Meshes/DaeAnimationExample/CharacterMultiple.dae");
	animator->SetAnimation("Idle");


	for (const auto& terrain : terrains)
	{
		auto grass_position = CreateGrassPositions(terrain, vec2(375, 550));

		auto grass_obj = ObjectBuilder::CreateGrass(*resourceManager_, grass_position, "Textures/Plants/G3_Nature_Plant_Grass_06_Diffuse_01.png");
		AddGameObject(grass_obj);
		renderersManager_->Subscribe(grass_obj);
	}

	characterController_ = std::make_shared<common::Controllers::CharacterController>(player->worldTransform, playerStats_.runSpeed, playerStats_.turnSpeed, playerStats_.jumpPower);
	playerInputController_ = std::make_shared<PlayerInputController>(animator, inputManager_, characterController_.get());
  
    dayNightCycle.SetDirectionalLight(&directionalLight);
    dayNightCycle.SetTime(.5f);

    camera = std::make_unique<GameEngine::FirstPersonCamera>(inputManager_, displayManager_);
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

	
	bool run = true;
	inputManager_->SubscribeOnKeyDown(KeyCodes::M, [&, run]() mutable
	{
		if (run)
		{
			player->GetComponent<GameEngine::Components::Animator>()->ChangeAnimation("Idle");
			run = false;
		}
		else
		{
			player->GetComponent<GameEngine::Components::Animator>()->ChangeAnimation("Run");
			run = true;
		}
	});

	inputManager_->SubscribeOnKeyDown(KeyCodes::MOUSE_WHEEL, [&]()
	{
		//auto d = camera->GetDistance() - 0.5f;
		//camera->SetDistance(d);
	});

	inputManager_->SubscribeOnKeyUp(KeyCodes::MOUSE_WHEEL, [&]()
	{
		//auto d = camera->GetDistance() + 0.5f;
		//camera->SetDistance(d);
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
			SetCamera(std::make_unique<GameEngine::ThirdPersonCamera>(inputManager_, &player->worldTransform));
		}
		else if (camType == CameraType::ThridPerson)
		{
			camType = CameraType::FirstPerson;
			SetCamera(std::make_unique<GameEngine::FirstPersonCamera>(inputManager_, displayManager_));
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
    auto terrain = ObjectBuilder::CreateTerrain(*resourceManager_, textures);
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
	//		auto obj = ObjectBuilder::CreateEntity(&resourceManager_, glm::vec3(0, 1.f, 0), "Meshes/Cube.obj");
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
	auto obj = ObjectBuilder::CreateEntity(resourceManager_.get(), modelName);
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

CGameObject* MainScene::AddGameObjectInstance(float scale, const vec2 & position, bool isDynamic)
{
	auto obj = new CGameObject();
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
	obj->worldTransform.TakeSnapShoot();
	return obj;
}

void MainScene::InitGui()
{

}
