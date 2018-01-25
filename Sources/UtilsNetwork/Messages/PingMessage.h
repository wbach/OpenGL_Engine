#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	struct PingMessage : public IMessage
	{
		PingMessage()
			: IMessage(MessageTypes::SelectCharacterReq, MessageTarget::Dedicated)
		{}

		uint32 lastPing = 0;

		virtual std::string ToString() override
		{
			return "Selected character ID: " + std::to_string(id);
		}
	};
} // Network
