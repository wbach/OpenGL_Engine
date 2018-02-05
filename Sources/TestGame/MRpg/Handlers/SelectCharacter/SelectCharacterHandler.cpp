#include "SelectCharacterHandler.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"

namespace MmmoRpg
{
	void SelectCharacterHandler::ProcessMessage(const Network::BoxMessage& message)
	{
		auto msg = Network::castMessageAs<Network::SelectCharacterMsgResp>(message.second.get());

		if (msg == nullptr)
		{
			Log("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " + std::to_string(msg->GetType()));
			return;
		}

		callback_(msg->id, msg->id, msg->status_ == Network::MessageStatus::Ok);
	}

} // MmmoRpg
