#include "MainScene.h"
#include "../GameEngine/Utils/GLM/GLMUtils.h"
#include "../GameEngine/Engine/Engine.h"
#include "../GameEngine/Objects/RenderAble/Grass.h"
#include "../GameEngine/Camera/FirstPersonCamera.h"
#include "../GameEngine/Camera/ThridPersonCamera.h"
#include "../GameEngine/Renderers/GUI/GuiRenderer.h"
#include "../GameEngine/Renderers/GUI/Text/GuiText.h"
#include "../GameEngine/Resources/Textures/Image.h"
#include "../GameEngine/Objects/RenderAble/Terrain.h"

MainScene::MainScene(CEngine &engine)
    : engine(engine)
{
	CGUIRenderer* gui_renderer = new CGUIRenderer();
	auto guiText = new CGuiText("../Data/GUI/consola.ttf", engine.m_Projection.GetWindowSize());
	gui_renderer->AddElement(guiText);

	SGuiTextElement score;
	score.text = "Lorem Ipsum jest tekstem stosowanym\njako przyk�adowy wype�niacz w przemy�le poligraficznym.\nZosta� po raz pierwszy u�yty w XV w.\nprzez nieznanego drukarza do wype�nienia tekstem pr�bnej ksi��ki.\nPi�� wiek�w p�niej zacz�� by� u�ywany przemy�le elektronicznym, pozostaj�c praktycznie niezmienionym. Spopularyzowa� si� w latach 60. XX w. wraz z publikacj� arkuszy Letrasetu, zawieraj�cych fragmenty Lorem Ipsum, a ostatnio z zawieraj�cym r�ne wersje Lorem Ipsum oprogramowaniem przeznaczonym do realizacji druk�w na komputerach osobistych, jak Aldus PageMaker.";
	score.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);

	score.position = glm::vec2(-0.9, 0.9);
	guiText->texts["Line_p1"] = score;

	engine.m_Renderers.emplace_back(gui_renderer);
	gui_renderer->Init();
}

MainScene::~MainScene()
{

}

int MainScene::Initialize()
{
    auto crate = AddGameObject(new CEntity(m_ResourceManager, glm::vec3(0, 2, 0), "../Data/Meshes/Crate/crate.obj"), glm::vec3(0,0, -5));
    engine.m_Renderers[0]->Subscribe(crate);

    player = new CPlayer(&engine.m_InputManager, m_ResourceManager, glm::vec3(0, 2, 0), "../Data/Meshes/Triss/Triss.obj"), glm::vec3(0, 0, 0);
	player->dynamic = true;
    AddGameObject(player);
    engine.m_Renderers[0]->Subscribe(player);

    auto small_house = AddGameObject(new CEntity(m_ResourceManager, glm::vec3(0,5,0), "../Data/Meshes/smallHouse1/smallHouse1.obj", "../Data/Example/monkey.obj", "../Data/Example/cube.obj"), glm::vec3(0.f, 0.f, -5.f));
    engine.m_Renderers[0]->Subscribe(small_house);

    auto terrain_textures = CreateTerrainTexturesMap();
    AddTerrain(terrain_textures, glm::vec3(1.f));
	//AddTerrain(terrain_textures, glm::vec3(-1.f, 1.f, 1.f));
   // AddGrass();

    m_DayNightCycle.SetDirectionalLight(&m_DirectionalLight);
    m_DayNightCycle.SetTime(.5f);

    //m_Camera = std::make_unique<CFirstPersonCamera>(&engine.m_InputManager, &engine.m_DisplayManager);

    m_Camera = std::make_unique<CThirdPersonCamera>(&engine.m_InputManager, player->m_WorldTransform);	

    return 0;
}

int MainScene::Update()
{
    if (m_Camera != nullptr)
    {
        m_Camera->CalculateInput();
        m_Camera->Move();
    }
    m_DeltaTime = static_cast<float>(engine.m_DisplayManager.GetDeltaTime());
    m_GloabalTime += m_DeltaTime;

    time_clock += m_DeltaTime;
    if(time_clock > 1.f)
    {
        int hour = 0, minutes = 0;
        m_DayNightCycle.GetCurrentHour(hour, minutes);
        std::cout << "Game Time : " << hour << " : " << minutes << "\n";
        Utils::PrintVector("Light position : ", this->m_DirectionalLight.GetPosition());
        time_clock = 0;
    }

    m_DayNightCycle.Update(m_DeltaTime); 

	for (auto& terrain : terrains)
	{
		auto new_position = terrain->CollisionDetection(player->m_WorldTransform.GetPosition());

		if (!new_position)
			continue;

		auto ppos = player->m_WorldTransform.GetPosition();
		if (ppos.y < new_position.GetValue().y)
			player->SetPosition(new_position.GetValue());
	}

	player->Move(m_DeltaTime);
	player->CheckInputs();

    return 0;
}

TerrainTexturesMap MainScene::CreateTerrainTexturesMap()
{
    return
    {
        { Terrain::blendMap , "../Data/Textures/testBlendMap.png"},
        { Terrain::backgorundTexture, "../Data/Textures/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
        { Terrain::redTexture, "../Data/Textures/165.png",  },
        { Terrain::greenTexture,"../Data/Textures/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        { Terrain::blueTexture, "../Data/Textures/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
        { Terrain::displacementMap, "../Data/Textures/heightmap.png" }
    };
}

void MainScene::AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position)
{
	//, CreateGrassPositions(), "../Data/Textures/G3_Nature_Plant_Grass_06_Diffuse_01.png")
	auto terrain = ObjectBuilder::CreateTerrain(m_ResourceManager, textures);

	if (terrain == nullptr)
	{
		Error("MainScene::AddTerrain : terrain is nullptr.");
		return;
	}	
    AddGameObject(terrain, position);
    engine.m_Renderers[0]->Subscribe(terrain);
	terrains.push_back(terrain);
}

std::vector<float> MainScene::CreateGrassPositions(CGameObject* object)
{
	std::vector<float> grass_positions;
	for (float y = 0.f; y < 200.f; y += 1.5f)
	{
		for (float x = 0.f; x < 200.f; x += 1.5f)
		{
			float xpos = x + ((rand() % 200 - 100) / 100.f);
			float zpos = y + ((rand() % 200 - 100) / 100.f);
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
 //   std::vector<unsigned short> indicies;
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
