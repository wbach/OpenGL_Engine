#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"

namespace Network
{
	struct DisconnectCharacterMsg : public IMessage
	{
		DisconnectCharacterMsg()
			: DisconnectCharacterMsg(0)
		{}
		DisconnectCharacterMsg(uint32 id)
			: IMessage(MessageTypes::DisconnectCharacter, MessageTarget::Dedicated)
			, id(id)
		{}

		uint32 id;

		virtual std::string ToString() override
		{
			return "Disconnect : " + std::to_string(id);
		}
	};
} // Network
