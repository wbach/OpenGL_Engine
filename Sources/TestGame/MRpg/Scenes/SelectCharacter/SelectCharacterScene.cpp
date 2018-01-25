#include "SelectCharacterScene.h"
#include "../UtilsNetwork/Gateway.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "../UtilsNetwork/Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "../UtilsNetwork/Messages/GetCharacters/GetCharactersMsgReq.h"
#include "../UtilsNetwork/Messages/GetCharacters/GetCharactersMsgResp.h"

namespace MmmoRpg
{
	SelectCharacterScene::SelectCharacterScene(GameEngine::CEngine & engine, Network::CGateway & gateway)
		: engine_(engine)
		, gateway_(gateway)
	{
	}
	SelectCharacterScene::~SelectCharacterScene()
	{
	}
	int SelectCharacterScene::Initialize()
	{
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
		Network::SelectCharacterMsgReq characterSelectReq;
		characterSelectReq.id = 101;
		gateway_.AddToOutbox(0, Network::CreateIMessagePtr<Network::SelectCharacterMsgReq>(characterSelectReq));
		Log("Wait for character select...");
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
	int SelectCharacterScene::Update(float deltaTime)
	{
		return 0;
	}
}
