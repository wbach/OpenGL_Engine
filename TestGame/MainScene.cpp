#include "MainScene.h"
#include "../GameEngine/Utils/GLM/GLMUtils.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "../GameEngine/Camera/FirstPersonCamera.h"
#include "../GameEngine/Camera/ThridPersonCamera.h"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Resources/Textures/Image.h"
#include "../GameEngine/Objects/RenderAble/Terrain/Terrain.h"

MainScene::MainScene(CEngine &engine)
    : engine(engine)
{
	CGUIRenderer* gui_renderer = new CGUIRenderer();
    auto guiText = new CGuiText("GUI/consola.ttf", engine.projection.GetWindowSize());
	gui_renderer->AddElement(guiText);

	SGuiTextElement score;
    score.text = "Lorem Ipsum jest tekstem stosowanym"; //njako przyk³adowy wype³niacz w przemyœle poligraficznym.\nZosta³ po raz pierwszy u¿yty w XV w.\nprzez nieznanego drukarza do wype³nienia tekstem próbnej ksi¹¿ki.\nPiêæ wieków póŸniej zacz¹³ byæ u¿ywany przemyœle elektronicznym, pozostaj¹c praktycznie niezmienionym. Spopularyzowa³ siê w latach 60. XX w. wraz z publikacj¹ arkuszy Letrasetu, zawieraj¹cych fragmenty Lorem Ipsum, a ostatnio z zawieraj¹cym ró¿ne wersje Lorem Ipsum oprogramowaniem przeznaczonym do realizacji druków na komputerach osobistych, jak Aldus PageMaker.";
	score.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

	score.position = glm::vec2(-0.9, 0.9);
	guiText->texts["Line_p1"] = score;

    engine.renderers.emplace_back(gui_renderer);
	gui_renderer->Init();
}

MainScene::~MainScene()
{
}

int MainScene::Initialize()
{
    auto crate_obj = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 2, 0), "Example/scene.dae");
    auto crate = AddGameObject(crate_obj, glm::vec3(0,0, -7));
    engine.renderers[0]->Subscribe(crate);

    auto crate_obj_2 = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 2, 0), "Meshes/Crate/crate.obj");
    auto crate_2 = AddGameObject(crate_obj_2, glm::vec3(10,0, -5));
    engine.renderers[0]->Subscribe(crate_2);

    player = new CPlayer(&engine.inputManager, resourceManager, glm::vec3(0, 2, 0), "Meshes/Triss/Triss.dae");
    player->dynamic = true;
    AddGameObject(player, glm::vec3(1,0,1));
    engine.renderers[0]->Subscribe(player);

    auto small_hause_obj = ObjectBuilder::CreateEntity(resourceManager, glm::vec3(0, 5, 0), "Meshes/smallHouse1/smallHouse1.obj", "Example/monkey.obj", "Example/cube.obj");
    auto small_house = AddGameObject(small_hause_obj, glm::vec3(15.f, 0.f, 35.f));
    engine.renderers[0]->Subscribe(small_house);

    auto terrain_textures = CreateTerrainTexturesMap();
    
  //  AddTerrain(terrain_textures, glm::vec3(1));

    //in terrain.h
    const float terrain_size = 200.f;
    for(float y = 0; y < 20*terrain_size; y+=terrain_size)
        for(float x = 0; x < 20*terrain_size; x+=terrain_size)
        {
            //if(x==0 || y==0) continue;
            AddTerrain(terrain_textures, glm::vec3(x, 1.f, y));
        }
   
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

    camera = std::make_unique<CFirstPersonCamera>(&engine.inputManager, &engine.displayManager);

    //camera = std::make_unique<CThirdPersonCamera>(&engine.inputManager, player->worldTransform);

    return 0;
}

int MainScene::Update()
{
    if (camera != nullptr)
    {
        camera->CalculateInput();
        camera->Move();
    }
    deltaTime = static_cast<float>(engine.displayManager.GetDeltaTime());
    gloabalTime += deltaTime;

    timeClock += deltaTime;
    if(timeClock > 1.f)
    {
        int hour = 0, minutes = 0;
        dayNightCycle.GetCurrentHour(hour, minutes);
        std::cout << "Game Time : " << hour << " : " << minutes << "\n";
        Utils::PrintVector("Light position : ", this->directionalLight.GetPosition());
        timeClock = 0;
    }

    dayNightCycle.Update(deltaTime);

    player->Move(deltaTime);
    player->CheckInputs();

	//for (auto& terrain : terrains)
	//{
 //       auto new_position = terrain->CollisionDetection(player->worldTransform.GetPosition());

	//	if (!new_position)
	//		continue;

 //       auto ppos = player->worldTransform.GetPosition();
	//	if (ppos.y < new_position.GetValue().y)
	//		player->SetPosition(new_position.GetValue());
	//}
    return 0;
}

TerrainTexturesMap MainScene::CreateTerrainTexturesMap()
{
    return
    {
        { Terrain::blendMap , "Textures/testBlendMap.png"},
        { Terrain::backgorundTexture, "Textures/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
        { Terrain::redTexture, "Textures/165.png",  },
        { Terrain::greenTexture,"Textures/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        { Terrain::blueTexture, "Textures/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
        { Terrain::displacementMap, "Textures/heightmap.png" }
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
    engine.renderers[0]->Subscribe(terr);
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
					ypos = height.GetValue().y;
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
