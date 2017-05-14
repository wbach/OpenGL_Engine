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
	score.text = "Lorem Ipsum jest tekstem stosowanym\njako przyk³adowy wype³niacz w przemyœle poligraficznym.\nZosta³ po raz pierwszy u¿yty w XV w.\nprzez nieznanego drukarza do wype³nienia tekstem próbnej ksi¹¿ki.\nPiêæ wieków póŸniej zacz¹³ byæ u¿ywany przemyœle elektronicznym, pozostaj¹c praktycznie niezmienionym. Spopularyzowa³ siê w latach 60. XX w. wraz z publikacj¹ arkuszy Letrasetu, zawieraj¹cych fragmenty Lorem Ipsum, a ostatnio z zawieraj¹cym ró¿ne wersje Lorem Ipsum oprogramowaniem przeznaczonym do realizacji druków na komputerach osobistych, jak Aldus PageMaker.";
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
    AddGrass();

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

    player->Move(m_DeltaTime);
    player->CheckInputs();

    if(terrain != nullptr)
    {
        auto height = terrain->GetHeightofTerrain(player->m_WorldTransform.GetPositionXZ());
        auto ppos = player->m_WorldTransform.GetPosition();
        if (ppos.y < height)
        {
            ppos.y = height;
            player->SetPosition(ppos);
        }
    }

    return 0;
}

std::map<STerrain::TexturesTypes, std::string> MainScene::CreateTerrainTexturesMap()
{
    return
    {
        { STerrain::blendMap , "../Data/Textures/testBlendMap.png"},
        { STerrain::backgorundTexture, "../Data/Textures/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
        { STerrain::redTexture, "../Data/Textures/165.png",  },
        { STerrain::greenTexture,"../Data/Textures/G3_Nature_Ground_Path_03_Diffuse_01.png"},
        { STerrain::blueTexture, "../Data/Textures/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
        { STerrain::displacementMap, "../Data/Textures/heightmap.png" }
    };
}

void MainScene::AddTerrain(std::map<STerrain::TexturesTypes, std::string> &textures, const glm::vec3& position)
{
    if(textures[STerrain::displacementMap].empty())
    {
        Error("Displacement map is not set while creating terrain.");
        return;
    }

    terrain = new STerrain();
    for(const auto& t : textures)
         terrain->SetTexture(m_ResourceManager.GetTextureLaoder().LoadTexture(t.second), t.first);

    SImage height_map;
    m_ResourceManager.GetTextureLaoder().ReadFile(textures[STerrain::displacementMap], height_map, TextureFlip::VERTICAL);
    terrain->LoadHeight(height_map);

    terrain->model = m_ResourceManager.LoadModel("../Data/Example/quad.obj");
    m_ResourceManager.GetOpenGlLoader().AddObjectToOpenGLLoadingPass(terrain->model);

    AddGameObject(terrain, position);
    engine.m_Renderers[0]->Subscribe(terrain);
}

void MainScene::AddGrass()
{
    std::vector<float> grass_position;
    std::vector<unsigned short> indicies;
    std::vector<float> empty_float_vec;
    std::vector<SVertexBoneData> empty_bones;
    SMaterial grass_material;

    for(float y = 0.f ; y < 200.f; y += 1.5f)
    {
        for (float x = 0.f; x < 200.f; x += 1.5f)
        {
            grass_position.push_back(x + ((rand() % 200 - 100) / 100.f));
            auto height = terrain != nullptr ? terrain->GetHeightofTerrain(x, y) : 0;
            grass_position.push_back(height + 1.f);
            grass_position.push_back(y + ((rand() % 200 - 100) / 100.f));
        }
    }

    SGrass* grass = new SGrass();
    grass->model = new CModel();
    grass_material.m_DiffuseTexture = m_ResourceManager.GetTextureLaoder().LoadTexture("../Data/Textures/G3_Nature_Plant_Grass_06_Diffuse_01.png");
    grass->model->AddMesh(grass_position, empty_float_vec, empty_float_vec, empty_float_vec, indicies, grass_material, empty_bones);
    m_ResourceManager.AddModel(grass->model);
    engine.m_Renderers[0]->Subscribe(grass);
}
