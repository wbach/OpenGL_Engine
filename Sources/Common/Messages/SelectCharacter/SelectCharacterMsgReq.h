#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	struct SelectCharacterMsgReq : public IMessage
	{
		SelectCharacterMsgReq()
			: IMessage(MessageTypes::SelectCharacterReq, MessageTarget::Dedicated)
		{}

		uint32 id = 0;

		virtual std::string ToString() override
		{
			return "Selected character ID: " + std::to_string(id);
		}
	};
} // Network
