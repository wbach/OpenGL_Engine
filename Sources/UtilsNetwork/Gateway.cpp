#include "Gateway.h"
#include "SDLNetWrapper.h"
#include "Logger/Log.h"
#include <SDL2/SDL_net.h>
#include "Thread.hpp"
#include <chrono>
#include "Messages/IMessage.h"
#include <functional>

namespace Network
{
	CGateway::CGateway()
		: connectionManager_(new SDLNetWrapper(), context_)
		, running(true)
		, timeMeasurer_(60)
		, isServer(false)
	{
	}
	CGateway::~CGateway()
	{
		running.store(false);
	}
	void CGateway::StartServer(uint32 maxClients, uint32 port)
	{
		context_ = serverCreator_.Create(maxClients, port);
		isServer = true;
		networkThread_ = std::thread(std::bind(&CGateway::MainLoop, this));
	}
	void CGateway::ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port)
	{
		context_ = clientCreator_.ConnectToServer(username, password, host, port);
		
		//from client pov is only one user - server

		context_.users[0] = std::make_shared<UtilsNetwork::UserData>();
		context_.users[0]->socket = context_.socket;

		recvThread_ = std::thread(std::bind(&CGateway::MainLoop, this));
	}	

	void CGateway::ReceiveAllMessages()
	{
		if (!connectionManager_.CheckSocketsActivity())
			return;
		
		if (isServer)
			connectionManager_.CheckNewConnectionsToServer();

		for (auto& user : context_.users)
		{
			auto msg = receiver_.Receive(user.second->socket);

			if (!msg) continue;

			for (auto& sub : onMessageArrivedSubcribes_)
			{
				sub({ user.first, msg });
			}
		}
	}

	void CGateway::SendAllMessages()
	{
		for (auto msg : outbox_)
		{
			auto socket = context_.users[msg.first]->socket;
			sender_.SendTcp(socket, msg.second.get());
		}
		ClearOutbox();
	}

	void CGateway::MainLoop()
	{
		timeMeasurer_.AddOnTickCallback(std::bind(&CGateway::PrintFps, this));

		while (running.load())
		{			
			ReceiveAllMessages();
			SendAllMessages();
			timeMeasurer_.CalculateAndLock();		
		}
	}

	void CGateway::ClearOutbox()
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.clear();
	}

	void CGateway::PrintFps()
	{
		std::string msg = "MainLoop fps : " + std::to_string(timeMeasurer_.GetFps());
		Log(msg);
	}

	void CGateway::AddToOutbox(uint32 userId, IMessagePtr message)
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.push_back({ userId , message });
	}

	//void CGateway::AddToOutbox(IMessagePtr message)
	//{
	//	std::lock_guard<std::mutex> lk(outboxMutex_);
	//	outbox_.push_back({ 0 , message });
	//}

	//std::shared_ptr<BoxMessage> CGateway::PopInBox()
	//{
	//	if (inbox_.empty())
	//		return nullptr;
	//	auto result = inbox_.front();
	//	return std::make_shared<BoxMessage>(result);
	//}

	void CGateway::SubscribeForNewUser(CreationFunc func)
	{
		connectionManager_.SubscribeForNewUser(func);
	}
	void CGateway::SubscribeOnMessageArrived(OnMessageArrived func)
	{
		onMessageArrivedSubcribes_.push_back(func);
	}
}