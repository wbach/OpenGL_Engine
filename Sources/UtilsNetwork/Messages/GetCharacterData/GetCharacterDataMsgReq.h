#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	struct GetCharacterDataMsgReq : public IMessage
	{
		GetCharacterDataMsgReq()
			: IMessage(MessageTypes::GetCharacterDataReq, MessageTarget::Dedicated)
		{}

		uint32 networkCharacterId;

		virtual std::string ToString() override
		{
			return "GetCharactersMsgReq";
		}
	};
} // Network