#include "SelectCharacterScene.h"
#include "../UtilsNetwork/Gateway.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "../UtilsNetwork/Messages/GetCharacters/GetCharactersMsgReq.h"
#include "../UtilsNetwork/Messages/GetCharacters/GetCharactersMsgResp.h"
#include "../GameEngine/Engine/Engine.h"

namespace MmmoRpg
{
	SelectCharacterScene::SelectCharacterScene(Network::CGateway& gateway)
		: CScene("SelectCharacterScene")
		, gateway_(gateway)
	{
		characterSelectText_.text = "SelectCharacterScene";
		characterSelectText_.colour = glm::vec3(0, 162.f / 255.f, 232.f / 255.f);
		characterSelectText_.position = glm::vec2(0, 0.0);
	}
	SelectCharacterScene::~SelectCharacterScene()
	{
		
	}
	int SelectCharacterScene::Initialize()
	{
		renderersManager_->GuiText("SelectCharacter") = characterSelectText_;

		return 0;

		Network::GetCharactersMsgReq getCharactersMsgReq;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::GetCharactersMsgReq>(getCharactersMsgReq));


		Log("Wait for get my characters.");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		auto myCharacters = gateway_.PopInBox();

		if (myCharacters)
		{
			auto msg = dynamic_cast<Network::GetCharactersMsgResp*>(myCharacters->second.get());
			if (msg)
			{
				Log("My characters : ");
				Log(msg->ToString());
				for (const auto& cd : msg->characterInfo)
				{
					if (!cd) continue;
					charactersData_.push_back(cd.value());
				}
			}
		}
		Log("SelectCharacterScene::Initialized");

		return 0;
	}
	void SelectCharacterScene::PostInitialize()
	{
		return;
		Network::SelectCharacterMsgReq characterSelectReq;
		characterSelectReq.id = 101;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::SelectCharacterMsgReq>(characterSelectReq));
		Log("Wait for character select...");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	int SelectCharacterScene::Update(float deltaTime)
	{
		if (!inputManager_)
		{
			Log("SelectCharacterScene::Update inputManager_ is null.");
			return 0;
		}
		if (inputManager_->GetKeyDown(KeyCodes::ENTER))
		{
			GameEngine::SceneEvent e(GameEngine::SceneEventType::LOAD_NEXT_SCENE, "MainScene");
			addSceneEvent(e);
		}
		return 0;
	}
}
