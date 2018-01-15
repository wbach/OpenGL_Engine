#pragma once
#include "Types.h"
#include "Sender.h"
#include "Reciever.h"
#include "Thread.hpp"
#include "Mutex.hpp"
#include "ConectContext.h"
#include "ConnectionManager.h"
#include "ServerCreator.h"
#include "ClientCreator.h"
#include <atomic>

namespace Network
{
	
	typedef std::pair<uint32, std::shared_ptr<IMessage>> BoxMessage;

	class CGateway
	{
	public:
		CGateway();
		~CGateway();
		void StartServer(uint32 maxClients, uint32 port);
		void ConnectToServer(const std::string& username, const std::string& password, uint32 port);
		void SubscribeForNewUser(CreationFunc func);
		void AddToOutbox(uint32 userId, std::shared_ptr<IMessage> message);
		std::shared_ptr<BoxMessage> PopInBox();

	private:
		void RunThreads();
		void ProccesSend();
		void ProccesRecv();
		void AddToInbox(uint32 userId, std::shared_ptr<IMessage> message);
		void ClearOutbox();

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
		bool isServer;

		std::mutex outboxMutex_;
		std::mutex inboxMutex_;
		std::list<BoxMessage> outbox_;
		std::list<BoxMessage> inbox_;
	};
}