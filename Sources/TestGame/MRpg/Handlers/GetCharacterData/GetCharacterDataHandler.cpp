#include "GetCharacterDataHandler.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include "TestGame/MRpg/Characters/NetworkCharacterManger.h"
#include "TestGame/MRpg/MrpgGameContext.h"

namespace MmmoRpg
{
	void GetCharacterDataHandler::ProcessMessage(const Network::BoxMessage& message)
	{
		auto msg = Network::castMessageAs<Network::GetCharacterDataMsgResp>(message.second.get());

		if (msg == nullptr)
		{
			Log("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " + std::to_string(msg->GetType()));
			return;
		}
		HandleNetworkCharacterMsg(*msg);
	}

	void GetCharacterDataHandler::HandleNetworkCharacterMsg(const Network::GetCharacterDataMsgResp& data)
	{
		networkCharacterManager_.AddCharacter(data.networkCharcterId, data.characterData.classId, data.position, data.commonStats);

		if (data.networkCharcterId == gameContext_.selectedCharacterId.first)
			gameContext_.selectedCharacterId.second = SelectedCharacterState::READY_TO_USE;
	}

} // MmmoRpg