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
#include "NetworkTypes.h"
#include <atomic>

namespace Network
{
	class CGateway
	{
	public:
		CGateway();
		~CGateway();
		void StartServer(uint32 maxClients, uint32 port);
		void ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port);
		void SubscribeForNewUser(CreationFunc func);
		//void SubscribeOnMessageArrived(OnMessageArrived func);
		void AddToOutbox(uint32 userId, IMessagePtr message);
		//void AddToOutbox(IMessagePtr message);
		std::shared_ptr<BoxMessage> PopInBox();

	private:
		void ReceiveAllMessages();
		void SendAllMessages();
		void MainLoop();
		void AddToInbox(uint32 userId, std::shared_ptr<IMessage> message);
		void ClearOutbox();
		void PrintFps();

	private:
		ISDLNetWrapperPtr iSDLNetWrapperPtr_;

		Sender sender_;
		Receiver receiver_;
		ConectContext context_;
		ConnectionManager connectionManager_;

		ServerCreator serverCreator_;
		ClientCreator clientCreator_;
		
		std::mutex outboxMutex_;
		std::mutex inboxMutex_;

		std::list<BoxMessage> outbox_;
		std::list<BoxMessage> inbox_;
		std::list<OnMessageArrived> onMessageArrivedSubcribes_;

		Utils::Time::CTimeMeasurer timeMeasurer_;
		bool isServer;
		std::thread  networkThread_;
		std::atomic_bool running;
	};

	typedef std::shared_ptr<CGateway> GatewayPtr;
}