#include "Gateway.h"
#include "SDLNetWrapper.h"
#include "Logger/Log.h"
#include <SDL2/SDL_net.h>
#include "Thread.hpp"
#include <chrono>
#include "Message.h"
#include <functional>

namespace Network
{
	CGateway::CGateway()
		: connectionManager_(new SDLNetWrapper(), context_)
		, running(true)
		, isServer(false)
	{
	}
	CGateway::~CGateway()
	{
		running.store(false);

		if (networkThread_.joinable())
			networkThread_.join();

		if (recvThread_.joinable())
			recvThread_.join();
	}
	void CGateway::StartServer(uint32 maxClients, uint32 port)
	{
		context_ = serverCreator_.Create(maxClients, port);
		isServer = true;
		RunThreads();
	}
	void CGateway::ConnectToServer(const std::string& username, const std::string& password, uint32 port)
	{
		context_ = clientCreator_.ConnectToServer(username, password, port);
		RunThreads();
	}
	
	void CGateway::RunThreads()
	{
		networkThread_ = std::thread(std::bind(&CGateway::ProccesSend, this));
		//recvThread_ = std::thread(std::bind(&CGateway::ProccesRecv, this));
	}

	void CGateway::ProccesSend()
	{
		while (running.load())
		{
			Log("CGateway::MainLoop.");

			if (isServer)
				connectionManager_.CheckNewConnectionsToServer();		

			if (!isServer)
			{
				auto msg = receiver_.Receive(context_.socket);
				AddToInbox(0, msg);
			}

			//if (!isServer) // tmp hack
			//for (auto& user : context_.users)
			//{
			//	auto msg = receiver_.Receive(user.second->socket);
			//	AddToInbox(user.first, msg);
			//}

			for (auto msg : outbox_)
			{
				auto socket = context_.users[msg.first]->socket;
				sender_.SendTcp(socket, msg.second.get());
			}
			ClearOutbox();

			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}

	void CGateway::ProccesRecv()
	{
		while (running.load())
		{
			for (auto& user : context_.users)
			{
				auto msg = receiver_.Receive(user.second->socket);
				AddToInbox(user.first, msg);
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}

	void CGateway::AddToInbox(uint32 userId, std::shared_ptr<IMessage> message)
	{
		std::lock_guard<std::mutex> lk(inboxMutex_);
		inbox_.push_back({ userId , message });
	}

	void CGateway::ClearOutbox()
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.clear();
	}

	void CGateway::AddToOutbox(uint32 userId, std::shared_ptr<IMessage> message)
	{
		std::lock_guard<std::mutex> lk(outboxMutex_);
		outbox_.push_back({ userId , message });
	}

	std::shared_ptr<BoxMessage> CGateway::PopInBox()
	{
		if (inbox_.empty())
			return nullptr;
		auto result = inbox_.front();
		return std::make_shared<BoxMessage>(result);
	}

	void CGateway::SubscribeForNewUser(CreationFunc func)
	{
		connectionManager_.SubscribeForNewUser(func);
	}

	/*void CGateway::ProccesSend()
	{
		TestData tdata;
		tdata.position = vec3(1.f, 2.f, 3.f);
		tdata.rotation = vec3(4.f, 5.f, 6.f);

		ConnectionMessage conMsg;
		conMsg.connectionStatus = ConnectionStatus::CONNECTED;

		while (running.load())
		{
			Log("CGateway::ServerMainLoop CheckNewConnectionsToServer.");
			connectionManager_.CheckNewConnectionsToServer();
			std::this_thread::sleep_for(std::chrono::milliseconds(200));

			for(auto& user : context_.users)
			{
				++tdata.id;				
				tdata.position += vec3(0.25, 0.1, 0.2);
				sender_.SendTcp(user->socket, &tdata);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(200));

			for (auto& user : context_.users)
			{
				sender_.SendTcp(user->socket, &conMsg);
			}
		}
	}
	void CGateway::ClientMainLoop()
	{
		while (running.load())
		{
			Log("CGateway::ClientMainLoop.");
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			receiver_.Receive(context_.socket);
		}
	}*/
}