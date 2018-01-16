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
#include "Time/TimeMeasurer.h"
#include <atomic>

namespace Network
{	
	typedef std::pair<uint32, std::shared_ptr<IMessage>> BoxMessage;
	typedef std::function<void(const BoxMessage&)> OnMessageArrived;

	class CGateway
	{
	public:
		CGateway();
		~CGateway();
		void StartServer(uint32 maxClients, uint32 port);
		void ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port);
		void SubscribeForNewUser(CreationFunc func);
		void SubscribeOnMessageArrived(OnMessageArrived func);
		void AddToOutbox(uint32 userId, std::shared_ptr<IMessage> message);
		void AddToOutbox(std::shared_ptr<IMessage> message);
		//std::shared_ptr<BoxMessage> PopInBox();

	private:
		void ReceiveAllMessages();
		void SendAllMessages();
		void MainLoop();
		//void AddToInbox(uint32 userId, std::shared_ptr<IMessage> message);
		void ClearOutbox();
		void PrintFps();

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
		Utils::Time::CTimeMeasurer timeMeasurer_;
		bool isServer;

		std::mutex outboxMutex_;
		std::mutex inboxMutex_;
		std::list<BoxMessage> outbox_;
		//std::list<BoxMessage> inbox_;
		std::list<OnMessageArrived> onMessageArrivedSubcribes_;
	};
}