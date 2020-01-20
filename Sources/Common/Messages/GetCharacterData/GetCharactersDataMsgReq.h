#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	struct GetCharactersDataMsgReq : public IMessage
	{
		GetCharactersDataMsgReq()
			: IMessage(MessageTypes::GetCharactersDataReq, MessageTarget::Dedicated)
		{}

		uint32 mapId;

		virtual std::string ToString() override
		{
			return "GetCharactersMsgReq";
		}
	};
} // Network