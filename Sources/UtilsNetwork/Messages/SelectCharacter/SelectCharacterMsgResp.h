#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "../MessageStatus.h"

namespace Network
{
	struct SelectCharacterMsgResp : public IMessage
	{
		SelectCharacterMsgResp()
			: IMessage(MessageTypes::SelectCharacterResp, MessageTarget::Dedicated)
		{}

		uint32 id = 0;
		uint32 mapId = 0;
		MessageStatus status_;

		virtual std::string ToString() override
		{
			return "Selected character ID: " + std::to_string(id) + " on map : " + std::to_string(mapId) + " status : " + (status_ == MessageStatus::Ok ? "Ok" : "Fail");
		}
	};
} // Network
