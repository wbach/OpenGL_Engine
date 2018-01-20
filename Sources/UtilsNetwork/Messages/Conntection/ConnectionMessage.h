#pragma once
#include "../IMessage.h"
#include "../MessageTarget.h"
#include "ConnectionStatus.h"

namespace Network
{
	struct ConnectionMessage : public IMessage
	{
		ConnectionMessage()
			: IMessage(MessageTypes::ConnectionMsg, MessageTarget::Dedicated)
		{}
		ConnectionMessage(ConnectionStatus status)
			: IMessage(MessageTypes::ConnectionMsg, MessageTarget::Dedicated)
			, connectionStatus(status)
		{}
		uint8 connectionStatus = 0;

		virtual std::string ToString() override
		{
			return "ConnectionStatus: " + std::to_string(connectionStatus);
		}
	};
} // Network
