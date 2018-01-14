#include "Gateway.h"
#include "SDLNetWrapper.h"
#include "Logger/Log.h"
#include <SDL2/SDL_net.h>
#include "Thread.hpp"
#include <chrono>
#include "Message.h"
#include <algorithm>
#include <functional>

namespace Network
{
	CGateway::CGateway()
		: connectionManager_(new SDLNetWrapper(), context_)
		, running(true)
	{
	}
	CGateway::~CGateway()
	{
		
	}

	void CGateway::Quit()
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
		networkThread_	= std::thread(std::bind(&CGateway::ServerMainLoop, this));
		//recvThread_		= std::thread(std::bind(&CGateway::ProccesRecv, this));

	}
	void CGateway::ConnectToServer(const std::string& username, const std::string& password, uint32 port)
	{
		context_ = clientCreator_.ConnectToServer(username, password, port);
		networkThread_ = std::thread(std::bind(&CGateway::ClientMainLoop, this));
	}
	void CGateway::ServerMainLoop()
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
	}
	void CGateway::ProccesRecv()
	{
		while (running.load())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}