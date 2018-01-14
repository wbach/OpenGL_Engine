#pragma once
#include "Types.h"
#include "Sender.h"
#include "Reciever.h"
#include "Thread.hpp"
#include "ConectContext.h"
#include "ConnectionManager.h"
#include "ServerCreator.h"
#include "ClientCreator.h"
#include <atomic>

namespace Network
{
	class CGateway
	{
	public:
		CGateway();
		~CGateway();
		void StartServer(uint32 maxClients, uint32 port);
		void ConnectToServer(const std::string& username, const std::string& password, uint32 port);
		void ServerMainLoop();
		void ClientMainLoop();

		void ProccesRecv();

		//void SubscribeForNewUser(uint32 userId);

	private:
		ConectContext context_;
		ConnectionManager connectionManager_;
		Sender sender_;
		Receiver receiver_;
		std::thread  networkThread_;
		std::thread  recvThread_;
		std::atomic_bool running;
		ServerCreator serverCreator_;
		ClientCreator clientCreator_;
	};
}