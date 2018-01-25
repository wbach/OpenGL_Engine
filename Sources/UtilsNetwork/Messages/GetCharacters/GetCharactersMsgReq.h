#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	struct GetCharactersMsgReq : public IMessage
	{
		GetCharactersMsgReq()
			: IMessage(MessageTypes::GetCharactersReq, MessageTarget::Dedicated)
		{}

		virtual std::string ToString() override
		{
			return "GetCharactersMsgReq";
		}
	};
} // Network