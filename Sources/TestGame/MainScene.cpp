#include "MainScene.h"
#include "SingleTon.h"
#include "../GameEngine/Engine/AplicationContext.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "../GameEngine/Camera/FirstPersonCamera.h"
#include "../GameEngine/Camera/ThridPersonCamera.h"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Resources/Textures/Image.h"
#include "../GameEngine/Objects/RenderAble/Terrain/Terrain.h"
#include "GLM/GLMUtils.h"
#include "Thread.hpp"

MainScene::MainScene(GameEngine::CEngine &engine)
    : CScene("MainScene")
	, engine(engine)
	, debuger(engine.inputManager)
{	
	InitGui();
	debuger.AddAction(KeyCodes::R, std::bind(&MainScene::ReloadShadersInRenderer, this));
}

MainScene::~MainScene()
{
	Log(__FUNCTION__);
}

int MainScene::Initialize()
{
	auto bialczyk_obj = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 2, 0), "Meshes/Bialczyk/Bialczyk.obj");
    auto bialczyk = AddGameObject(bialczyk_obj, glm::vec3(100, 17, -7));
	engine.renderersManager_.Subscribe(bialczyk);

	//auto character_running_obj = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 2, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
	//auto ch = AddGameObject(character_running_obj, glm::vec3(0, 0, -7));
	//engine.renderers[0]->Subscribe(ch);

 //   auto crate_obj_2 = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 2, 0), "Meshes/Crate/crate.obj");
 //   auto crate_2 = AddGameObject(crate_obj_2, glm::vec3(10,0, -5));
 //   engine.renderers[0]->Subscribe(crate_2);

    player = new CPlayer(&engine.inputManager, resourceManager, glm::vec3(0, 1.8, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
    player->dynamic = true;
    AddGameObject(player, glm::vec3(1,0,1));
	engine.renderersManager_.Subscribe(player);

    //auto small_hause_obj = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 5, 0), "Meshes/smallHouse1/smallHouse1.obj", "Example/monkey.obj", "Example/cube.obj");
    //auto small_house = AddGameObject(small_hause_obj, glm::vec3(15.f, 0.f, 35.f));
    //engine.renderers[0]->Subscribe(small_house);

    auto terrain_textures = CreateTerrainTexturesMap();

  //  AddTerrain(terrain_textures, glm::vec3(1));

	//const float terrains_count = 2;
 //   for(float y = 0; y < terrains_count*TERRAIN_SIZE; y+= TERRAIN_SIZE)
 //       for(float x = 0; x < terrains_count*TERRAIN_SIZE; x+= TERRAIN_SIZE)
 //       {
 //           //if(x==0 || y==0) continue;
 //           AddTerrain(terrain_textures, glm::vec3(x, -50.f, y));
 //       }
   
   // AddGrass();

  /*  for (const auto& terrain : terrains)
    {
        auto grass_position = CreateGrassPositions(terrain);

        auto grass_obj = ObjectBuilder::CreateGrass(resourceManager, grass_position, "Textures/G3_Nature_Plant_Grass_06_Diffuse_01.png");
        AddGameObject(grass_obj);
        engine.renderers[0]->Subscribe(grass_obj);
    }
*/
    dayNightCycle.SetDirectionalLight(&directionalLight);
    dayNightCycle.SetTime(.5f);

  // camera = std::make_unique<CFirstPersonCamera>(&engine.inputManager, &engine.GetDisplayManager(), deltaTime);

    camera = std::make_unique<CThirdPersonCamera>(&engine.inputManager, player->worldTransform);

	if (camera != nullptr)
	{
		camera->CalculateInput();
		camera->Move();
	}

    return 0;
}
//#include <stdio.h>
int MainScene::Update(float dt)
{
	if (camera == nullptr)
	{
	   Log("MainScene::Update camera is nullptr.");
	   return -1;
	}

	deltaTime = dt;
    gloabalTime += deltaTime;

    timeClock += deltaTime;    
    if(timeClock > 1.f)
    {
        int hour = 0, minutes = 0;
        dayNightCycle.GetCurrentHour(hour, minutes);
        timeClock = 0;
        Log("Game Time : " + std::to_string(hour) + ":" + std::to_string(minutes));
    }

    dayNightCycle.Update(deltaTime);
	
    CheckCollisions();
	DebugRenderOptionsControl();
	UpdatePlayerandCamera(deltaTime);

    return 0;
}

void MainScene::UpdatePlayerandCamera(float time)
{
	camera->CalculateInput();
	std::lock_guard<std::mutex>(SingleTon<GameEngine::SAplicationContext>::Get().renderingMutex);
    player->Update(time);
    camera->Move();
}

void MainScene::CheckCollisions()
{
	for (auto& terrain : terrains)
	{
        auto new_position = terrain->CollisionDetection(player->worldTransform.GetPosition());

		if (!new_position)
			continue;

        auto ppos = player->worldTransform.GetPosition();
		if (ppos.y < new_position.value().y)
			player->SetPosition(new_position.value());
	}
}

TerrainTexturesMap MainScene::CreateTerrainTexturesMap()
{
    return
    {
        { Terrain::blendMap , "Textures/Terrain/BlendMaps/testBlendMap.png"},
        { Terrain::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
        { Terrain::redTexture, "Textures/Terrain/Ground/165.png",  },
        { Terrain::greenTexture,"Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        { Terrain::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
        { Terrain::displacementMap, "Textures/Terrain/HeightMaps/heightmap.png" }
    };
}

void MainScene::AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position)
{
	//, CreateGrassPositions(), "../Data/Textures/G3_Nature_Plant_Grass_06_Diffuse_01.png")
    auto terrain = ObjectBuilder::CreateTerrain(resourceManager, textures);
	if (terrain == nullptr)
	{
		Error("MainScene::AddTerrain : terrain is nullptr.");
		return;
    }    
    auto terr = AddGameObject(terrain, position);
    terrains.push_back(terr);
    engine.renderersManager_.Subscribe(terr);
}

std::vector<float> MainScene::CreateGrassPositions(CGameObject* object)
{
	std::vector<float> grass_positions;
	for (float y = 0.f; y < 200.f; y += 10.5f)
	{
		for (float x = 0.f; x < 200.f; x += 10.5f)
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

	//if (terrain == nullptr)
	//{
	//	Error("MainScene::AddGrass : terrain is nullptr.");
	//	return;
	//}

 //   std::vector<float> grass_position;
 //   std::vector<uint16> indicies;
 //   std::vector<float> empty_float_vec;
 //   std::vector<SVertexBoneData> empty_bones;
 //   SMaterial grass_material;

 //   for(float y = 0.f ; y < 200.f; y += 1.5f)
 //   {
 //       for (float x = 0.f; x < 200.f; x += 1.5f)
 //       {
 //           grass_position.push_back(x + ((rand() % 200 - 100) / 100.f));
 //           auto height = terrain != nullptr ? terrain->GetHeightofTerrain(x, y) : 0;
 //           grass_position.push_back(height + 1.f);
 //           grass_position.push_back(y + ((rand() % 200 - 100) / 100.f));
 //       }
 //   }

 //   SGrass* grass = new SGrass();
 //   grass->model = new CModel();
 //   grass_material.m_DiffuseTexture = m_ResourceManager.GetTextureLaoder().LoadTexture("../Data/Textures/G3_Nature_Plant_Grass_06_Diffuse_01.png");
 //   grass->model->AddMesh(grass_position, empty_float_vec, empty_float_vec, empty_float_vec, indicies, grass_material, empty_bones);
 //   m_ResourceManager.AddModel(grass->model);
 //   engine.m_Renderers[0]->Subscribe(grass);
}

void MainScene::InitGui()
{
	SGuiTextElement score;
	score.text =
		"V -  on/off debug keys."
		"G -  on/off grid";
	score.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

	score.position = glm::vec2(-0.9, 0.9);
	//engine.gui.texts->texts["Line_p1"] = score;	

	//debuger.SetGuiRenderer(engine.gui.renderer);
	//debuger.Init();
	//gui_renderer->Init();
}

void MainScene::DebugRenderOptionsControl()
{
	if (engine.inputManager.GetKeyDown(KeyCodes::V))
	{
		debuger.TurnOnOff();
	}
	debuger.Execute();
}

void MainScene::ReloadShadersInRenderer()
{
	/*for (auto& renderer : engine.renderers)
	{
		renderer->ReloadShaders();
	}*/
}
