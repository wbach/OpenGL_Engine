#pragma once
#include "Database/IDatabaseWrapper.h"
#include "Message.h"

namespace GameServer
{
	typedef std::function<void(uint32, Network::IMessagePtr)> SendMessage;

	struct Context
	{
		Context() 
		{
		}
		Context(Database::IDatabaseWrapperPtr databaseWrapperPtr, SendMessage sendMessage)
			: databaseWrapper_(databaseWrapperPtr)
			, sendMessage_(sendMessage)
		{}

		Database::IDatabaseWrapperPtr databaseWrapper_;
		SendMessage sendMessage_;
	};
}