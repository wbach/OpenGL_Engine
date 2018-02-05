#include "DistributeHandler.h"
#include "Messages/MessageTarget.h"

namespace GameServer
{
	namespace Handler
	{
		void DistributeHandler::ProcessMessage(const Network::BoxMessage& message)
		{
			if (!message.second)
				return;

			switch (message.second->GetTarget())
			{
			case Network::MessageTarget::All:
				for (auto& user : context_.GetUsers())
					context_.sendMessage_(message.first, message.second.get());
				break;
			}		
		}

	} // Handler
} // GameServer