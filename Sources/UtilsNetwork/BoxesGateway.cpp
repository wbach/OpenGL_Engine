#include "BoxesGateway.h"
#include "SDLNetWrapper.h"
#include "Logger/Log.h"
#include <SDL2/SDL_net.h>
#include "Thread.hpp"
#include <chrono>
#include "Messages/IMessage.h"
#include <functional>

namespace Network
{
	BoxesGateway::BoxesGateway()
		: CGateway(Utils::Time::CTimeMeasurer(120, true))
		, threadCreated(false)
		, running(false)
	{
		SubscribeOnMessageArrived("AddToInbox", std::bind(&BoxesGateway::AddToInbox, this, std::placeholders::_1));
	}
	BoxesGateway::~BoxesGateway()
	{
		outbox_.clear();
		inbox_.clear();
		running.store(false);

		if (threadCreated || networkThread_.joinable())
			networkThread_.join();
	}
	void BoxesGateway::StartServer(uint32 maxClients, uint32 port)
	{
		CGateway::StartServer(maxClients, port);
		running.store(true);
		CreateThread();
	}
	bool BoxesGateway::ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port)
	{
		if(!CGateway::ConnectToServer(username, password, host, port))
			return false;
		
		CreateThread();
		return true;
	}

	void BoxesGateway::CreateThread()
	{
		threadCreated = true;
		running.store(true);
		networkThread_ = std::thread(std::bind(&BoxesGateway::MainLoop, this));
	}	

	void BoxesGateway::SendAllMessages()
	{
		while (true)
		{
			auto msg = PopOutBox();
			if (msg == nullptr)
				break;

			//DEBUG_LOG("BoxesGateway::SendAllMessages() " + Network::to_string(msg->second->GetType()) + ".");

			auto& user = context_.users[msg->first];
			auto socket = user->socket;
			auto status = sender_.SendTcp(socket, msg->second.get());

			if (status != SentStatus::OK)
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
				DEBUG_LOG("Sent " + Network::to_string(msg->second->GetType()) + " failed.");
			}
			else
			{
				user->connectionFailsStart = wb::optional<Timepoint>();
			}
		}
	}

	void BoxesGateway::MainLoop()
	{
		timeMeasurer_.AddOnTickCallback(std::bind(&BoxesGateway::PrintFps, this));

		while (running.load())
		{
			Update();
			SendAllMessages();
			timeMeasurer_.CalculateAndLock();
		}
	}

	void BoxesGateway::AddToInbox(const BoxMessage& message)
	{
		std::lock_guard<std::mutex> l(inboxMutex_);
		inbox_.push_back(message);
	}

	uint32 BoxesGateway::GetOutBoxSize()
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		auto i = outbox_.size();
		return i;
	}

	std::shared_ptr<BoxMessage> BoxesGateway::PopOutBox()
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		if (outbox_.empty())
			return nullptr;
		auto result = outbox_.front();
		outbox_.pop_front();
		return std::make_shared<BoxMessage>(result);
	}

	void BoxesGateway::ClearOutbox()
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.clear();
	}

	void BoxesGateway::PrintFps()
	{
		std::string msg = "Network thread fps : " + std::to_string(timeMeasurer_.GetFps());
		DEBUG_LOG(msg);
	}

	void BoxesGateway::AddToOutbox(uint32 userId, IMessagePtr message)
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.push_back({ userId , message });
	}

	void BoxesGateway::AddToOutbox(IMessagePtr message)
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.push_back({ 0 , message });
	}

	std::shared_ptr<BoxMessage> BoxesGateway::PopInBox()
	{
		std::lock_guard<std::mutex> l(inboxMutex_);
		if (inbox_.empty())
			return nullptr;
		auto result = inbox_.front();
		inbox_.pop_front();
		return std::make_shared<BoxMessage>(result);
	}
}