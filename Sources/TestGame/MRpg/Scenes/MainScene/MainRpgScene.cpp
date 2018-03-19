#include "MainRpgScene.h"
#include "Camera/FirstPersonCamera.h"
#include "Camera/ThridPersonCamera.h"
#include "GameEngine/Objects/ObjectBuilder.h"
#include "GameEngine/Renderers/RenderersManager.h"
#include "TestGame/MRpg/Characters/PlayerController.h"
#include "UtilsNetwork/Messages/RemoveCharacter/DisconnectCharacterMsg.h"
#include "UtilsNetwork/Messages/TransformMessages/TransformMessageTypes.h"
#include "UtilsNetwork/Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "TestGame/MRpg/Handlers/GetCharacterData/GetCharacterDataHandler.h"
#include "TestGame/MRpg/Handlers/DisconnectCharacter/DisconnectHandler.h"
#include "TestGame/MRpg/Handlers/Transform/TransformHandler.h"
#include "SingleTon.h"
#include "GameEngine/Engine/AplicationContext.h"

namespace MmmoRpg
{
	MainRpgScene::MainRpgScene(Network::CGateway& gateway, const std::string& serverAddress, MrpgGameContext& gameContext)
		: MRpgScene("MainRpgScene", gateway, serverAddress, gameContext)
	{
	}

	MainRpgScene::~MainRpgScene()
	{
		Log(__FUNCTION__);
	}

	int MainRpgScene::Initialize()
	{
		Log("MainRpgScene::Initialize()");

		modelsCreator_ = std::make_unique<ModelsCreator>(resourceManager_.get());
		networkCharacterManager_ = std::make_unique<NetworkCharacterManager>(modelsCreator_.get(), *renderersManager_, gameContext_, std::bind(&MainRpgScene::AddGameObject, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		networkCharacterManager_->SubscribeOnGetPlayer(std::bind(&MainRpgScene::OnGetPlayer, this, std::placeholders::_1));

		dispatcher_.AddHandler("GetCharacterDataHandler", new GetCharacterDataHandler(*networkCharacterManager_));
		dispatcher_.AddHandler("DisconnectHandler", new DisconnectHandler(*networkCharacterManager_));
		dispatcher_.AddHandler("TransformHandler", new TransformHandler(*networkCharacterManager_));

		ReqNetworkSceneCharacters();
		
		auto bialczyk_obj = ObjectBuilder::CreateEntity(resourceManager_.get(), "Meshes/Bialczyk/Bialczyk.obj");
		AddGameObject(bialczyk_obj, glm::vec3(100, 17, -7));
		renderersManager_->Subscribe(bialczyk_obj);

		
		//camera = std::make_unique<CFirstPersonCamera>(inputManager_, displayManager_);
		playerController_ = std::make_shared<PlayerController>(inputManager_, gameContext_, gateway_);


		dayNightCycle.SetDirectionalLight(&directionalLight);
		dayNightCycle.SetTime(.5f);
		return 0;
	}

	int MainRpgScene::Update(float dt)
	{
		gateway_.Update();
		networkCharacterManager_->Update(dt);
		dayNightCycle.Update(dt);
		UpdatePlayerandCamera(dt);
		return 0;
	}

	void MainRpgScene::UpdatePlayerandCamera(float time)
	{
		camera->CalculateInput();
		camera->Move();
	}

	void MainRpgScene::ReqNetworkSceneCharacters()
	{
		Log("Send request");
		auto characterMsgReq = std::make_unique<Network::GetCharactersDataMsgReq>();
		characterMsgReq->mapId = 1;// hack id : 1
		gateway_.Send(characterMsgReq.get());
	}

	void MainRpgScene::OnGetPlayer(NetworkCharacter* character)
	{
		camera = std::make_unique<CThirdPersonCamera>(inputManager_, &character->GetEntity()->worldTransform);
	}

	/*void MainRpgScene::HandleTransformMsg(std::shared_ptr<Network::TransformMsgResp> msg)
	{
		Log("Times test : Resp: " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC) + " Action: " + std::to_string(msg->action));

		auto characterId = msg->id;
		Log("msg->position : " + Utils::ToString(msg->position) + " msg->rotation : " + Utils::ToString(msg->rotation));
		networkCharacters_[msg->id]->GetEntity()->worldTransform.SetPosition(msg->position);
		networkCharacters_[msg->id]->GetEntity()->worldTransform.SetRotation(msg->rotation);
		auto icontroller = networkCharacters_[msg->id]->GetControllerByType(common::Controllers::Types::CharacterControllerType);
		auto controller = common::Controllers::castControllerAs<common::Controllers::CharacterController>(icontroller);
		Log("Controller->position : " + Utils::ToString(controller->GetTransform().GetPosition()) + " Controller->rotation : " + Utils::ToString(controller->GetTransform().GetRotation()));

		switch (msg->action)
		{
		case Network::TransformAction::PUSH:
			controller->AddState(common::Controllers::NetworkActionsConverter::Convert(msg->type));
			break;
		case Network::TransformAction::POP:
			controller->RemoveState(common::Controllers::NetworkActionsConverter::Convert(msg->type));
			break;
		}		
	}*/
} // MmmoRpg
