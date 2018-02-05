#pragma once
#include "Types.h"
#include "Gateway.h"
#include "Thread.hpp"
#include "Mutex.hpp"
#include <atomic>

namespace Network
{
	class BoxesGateway : public CGateway
	{
	public:
		BoxesGateway();
		~BoxesGateway();
		void StartServer(uint32 maxClients, uint32 port);
		bool ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port);
		void CreateThread();
		void AddToOutbox(IMessagePtr message);
		void AddToOutbox(uint32 userId, IMessagePtr message);
		std::shared_ptr<BoxMessage> PopInBox();

	private:
		void SendAllMessages();
		void MainLoop();
		void AddToInbox(const BoxMessage&);
		uint32 GetOutBoxSize();
		std::shared_ptr<BoxMessage> PopOutBox();
		void ClearOutbox();
		void PrintFps();

	private:
		std::mutex outboxMutex_;
		std::mutex inboxMutex_;

		std::list<BoxMessage> outbox_;
		std::list<BoxMessage> inbox_;

		bool threadCreated;
		std::thread  networkThread_;
		std::atomic_bool running;
	};

	typedef std::shared_ptr<BoxesGateway> BoxesGatewayPtr;
}