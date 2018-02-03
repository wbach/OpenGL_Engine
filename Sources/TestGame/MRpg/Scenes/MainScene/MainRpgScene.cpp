#include "MainRpgScene.h"
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
#include "../GameEngine/Resources/Models/ModelFactory.h"

#include "../../Characters/PlayerController.h"
#include "../../../../UtilsNetwork/Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "../../../../Common/Hero/HeroClassesTypes.h"

#include "GLM/GLMUtils.h"
#include "Thread.hpp"

namespace MmmoRpg
{
	MainRpgScene::MainRpgScene(Network::CGateway& gateway, MrpgGameContext& gameContext)
		: GameEngine::Scene("MainRpgScene")
		, gateway_(gateway)
		, gameContext_(gameContext)
	{
	}

	MainRpgScene::~MainRpgScene()
	{
		Log(__FUNCTION__);
	}

	int MainRpgScene::Initialize()
	{
		ReqNetworkSceneCharacters();
		
		Log("MainRpgScene::Initialize()");
		auto bialczyk_obj = ObjectBuilder::CreateEntity(&resourceManager, glm::vec3(0, 2, 0), "Meshes/Bialczyk/Bialczyk.obj");
		auto bialczyk = AddGameObject(bialczyk_obj, glm::vec3(100, 17, -7));
		renderersManager_->Subscribe(bialczyk);

		dayNightCycle.SetDirectionalLight(&directionalLight);
		dayNightCycle.SetTime(.5f);

		//camera = std::make_unique<CThirdPersonCamera>(inputManager_, player->worldTransform);
		camera = std::make_unique<CFirstPersonCamera>(inputManager_, displayManager_, deltaTime);

		if (camera != nullptr)
		{
			camera->CalculateInput();
			camera->Move();
		}

		inputManager_->SubscribeOnKeyDown(KeyCodes::ENTER, [&]()
		{
			GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_SCENE_BY_NAME, "MainRpgScene");
			addSceneEvent(e);
		});

		return 0;
	}

	int MainRpgScene::Update(float dt)
	{
		CheckIncomingMessages();
		ControlPlayer();

		if (camera == nullptr)
		{
			Log("MainRpgScene::Update camera is nullptr.");
			return -1;
		}

		deltaTime = dt;
		gloabalTime += deltaTime;

		timeClock += deltaTime;
		if (timeClock > 1.f)
		{
			int hour = 0, minutes = 0;
			dayNightCycle.GetCurrentHour(hour, minutes);
			timeClock = 0;
			Log("Game Time : " + std::to_string(hour) + ":" + std::to_string(minutes));
		}		

		dayNightCycle.Update(deltaTime);

		CheckCollisions();
		UpdatePlayerandCamera(deltaTime);

		return 0;
	}

	void MainRpgScene::UpdatePlayerandCamera(float time)
	{
		camera->CalculateInput();
		std::lock_guard<std::mutex>(SingleTon<GameEngine::SAplicationContext>::Get().renderingMutex);
		//player->Update(time);
		camera->Move();
	}

	void MainRpgScene::CheckCollisions()
	{
		//for (auto& terrain : terrains)
		//{
		//	auto new_position = terrain->CollisionDetection(player->worldTransform.GetPosition());

		//	if (!new_position)
		//		continue;

		//	auto ppos = player->worldTransform.GetPosition();
		//	if (ppos.y < new_position.value().y)
		//		player->SetPosition(new_position.value());
		//}
	}

	TerrainTexturesMap MainRpgScene::CreateTerrainTexturesMap()
	{
		return
		{
			{ Terrain::blendMap , "Textures/Terrain/BlendMaps/testBlendMap.png" },
			{ Terrain::backgorundTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Grass_01_Diffuse_01.png" },
			{ Terrain::redTexture, "Textures/Terrain/Ground/165.png", },
			{ Terrain::greenTexture,"Textures/Terrain/Ground/G3_Nature_Ground_Path_03_Diffuse_01.png" },
			{ Terrain::blueTexture, "Textures/Terrain/Ground/G3_Nature_Ground_Forest_01_Diffuse_01.png" },
			{ Terrain::displacementMap, "Textures/Terrain/HeightMaps/heightmap.png" }
		};
	}

	void MainRpgScene::AddTerrain(TerrainTexturesMap& textures, const glm::vec3& position)
	{
		auto terrain = ObjectBuilder::CreateTerrain(resourceManager, textures);
		if (terrain == nullptr)
		{
			Error("MainRpgScene::AddTerrain : terrain is nullptr.");
			return;
		}
		auto terr = AddGameObject(terrain, position);
		terrains.push_back(terr);
		renderersManager_->Subscribe(terr);
	}

	std::vector<float> MainRpgScene::CreateGrassPositions(CGameObject* object)
	{
		return {};
	}

	void MainRpgScene::ReqNetworkSceneCharacters()
	{
		Network::GetCharactersDataMsgReq characterMsgReq;
		characterMsgReq.mapId = 1;// hack id : 1
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::GetCharactersDataMsgReq>(characterMsgReq));
	}

	void MainRpgScene::WaitForNetworkCharacters()
	{
	}

	void MainRpgScene::CheckIncomingMessages()
	{
		auto msg = gateway_.PopInBox();
		if (msg == nullptr)
			return;

		Log(" MainRpgScene::CheckIncomingMessages Got msg type : " + std::to_string(msg->second->GetType()));

		switch (msg->second->GetType())
		{
			case Network::MessageTypes::GetCharacterDataResp:
			{
				auto getCharacterDataResp = Network::castMessageAs<Network::GetCharacterDataMsgResp>(msg->second);
				if (getCharacterDataResp == nullptr)
					return;
				HandleNetworkCharacterMsg(getCharacterDataResp);
			}

			break;
		}
	}

	void MainRpgScene::HandleNetworkCharacterMsg(std::shared_ptr<Network::GetCharacterDataMsgResp> msg)
	{
		//NetworkCharacter(uint32 id, const vec3& scale, GameEngine::ModelWrapper modelWrapper);
		GameEngine::ModelWrapper modelWrapper;
		vec3 normalizedSize;

	/*	switch (msg->characterData.classId)
		{
		case common::Hero::HeroClassesTypes::KNIGHT: 
			break;
		case common::Hero::HeroClassesTypes::ELF:
			break;
		case common::Hero::HeroClassesTypes::WIZARD:
			break;
		default:
		{*/
			auto model = GameEngine::LoadModel(&resourceManager, vec3(0, 2, 0), "Meshes/DaeAnimationExample/CharacterRunning.dae");
			normalizedSize = model->GetNormalizedScaleVector();
			modelWrapper.Add(model, GameEngine::LevelOfDetail::L1);
	/*	}		
		break;
		}*/

		networkCharacters_[msg->networkCharcterId] = std::make_shared<NetworkCharacter>(msg->networkCharcterId, normalizedSize, msg->commonStats, modelWrapper);
		
		auto entity = networkCharacters_[msg->networkCharcterId]->GetEntity();
		entity->dynamic = true;
		AddGameObject(entity, msg->position);
		renderersManager_->Subscribe(entity);

		if (msg->networkCharcterId == gameContext_.selectedCharacterId)
		{
			camera = std::make_unique<CThirdPersonCamera>(inputManager_, entity->worldTransform);
			playerController_ = std::make_shared<PlayerController>(inputManager_, gameContext_.selectedCharacterId, gateway_);
		}
	}

	void MainRpgScene::ControlPlayer()
	{
		if (playerController_ == nullptr)
			return;

		playerController_->Control();
	}

	void MainRpgScene::InitGui()
	{

	}
} // MmmoRpg
