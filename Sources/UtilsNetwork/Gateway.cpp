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
		: iSDLNetWrapperPtr_(new SDLNetWrapper)
		, connectionManager_(iSDLNetWrapperPtr_, context_)
		, clientCreator_(iSDLNetWrapperPtr_)
		, running(true)
		, timeMeasurer_(60)
		, isServer(false)
		, sender_(iSDLNetWrapperPtr_)
		, receiver_(iSDLNetWrapperPtr_)
	{
	}
	CGateway::~CGateway()
	{
		outbox_.clear();
		inbox_.clear();
		running.store(false);
		if(networkThread_.joinable())
		networkThread_.join();
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

		networkThread_ = std::thread(std::bind(&CGateway::MainLoop, this));
	}	

	void CGateway::ReceiveAllMessages()
	{
		if (!connectionManager_.CheckSocketsActivity())
			return;
		
		if (isServer)
			connectionManager_.CheckNewConnectionsToServer();


		for (auto iter = context_.users.begin(); iter != context_.users.end();)
		{
			auto& user = *iter;

			RecvError err;
			auto msg = receiver_.Receive(user.second->socket, err);

			if (err == RecvError::Disconnect)
			{
				connectionManager_.DisconectUser(user.second->id);
				iter = context_.users.erase(iter);
			}
			else
			{
				++iter;
			}

			if (!msg) continue;
			
			AddToInbox(user.first, msg);

			/*for (auto& sub : onMessageArrivedSubcribes_)
			{
				sub({ user.first, msg });
			}*/
		}
	}

	void CGateway::SendAllMessages()
	{
		for (auto msg : outbox_)
		{
			auto& user = context_.users[msg.first];
			auto socket = user->socket;

			if (sender_.SendTcp(socket, msg.second.get()) == SentStatus::ERROR)
			{
				if (!user->connectionFailsStart)
				{
					user->connectionFailsStart = std::chrono::high_resolution_clock::now();
				}
				else
				{
					auto dt = std::chrono::high_resolution_clock::now() - user->connectionFailsStart.value();
					auto t = std::chrono::duration_cast<std::chrono::seconds>(dt);
					if (t > std::chrono::seconds(2))
					{
						connectionManager_.DisconectUser(user->id);
					}
				}
			}
			else
			{
				user->connectionFailsStart = wb::optional<Timepoint>();
			}
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

	void CGateway::AddToInbox(uint32 userId, std::shared_ptr<IMessage> message)
	{
		std::lock_guard<std::mutex> l(inboxMutex_);
		inbox_.push_back({ userId , message });
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

	std::shared_ptr<BoxMessage> CGateway::PopInBox()
	{
		std::lock_guard<std::mutex> l(inboxMutex_);
		if (inbox_.empty())
			return nullptr;
		auto result = inbox_.front();
		inbox_.pop_front();
		return std::make_shared<BoxMessage>(result);
	}

	void CGateway::SubscribeForNewUser(CreationFunc func)
	{
		connectionManager_.SubscribeForNewUser(func);
	}
	/*void CGateway::SubscribeOnMessageArrived(OnMessageArrived func)
	{
		onMessageArrivedSubcribes_.push_back(func);
	}*/
}