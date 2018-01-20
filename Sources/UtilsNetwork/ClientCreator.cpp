#include "ClientCreator.h"
#include "Utils.h"
#include "Logger/Log.h"
#include "Reciever.h"
#include "Sender.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/Conntection/AuthenticationMessage.h"

namespace Network
{
	ClientCreator::ClientCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
	{
	}

	ClientCreator::ClientCreator(ISDLNetWrapper * sdlNetWrapper)
	{		
	}

	ConectContext ClientCreator::ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port)
	{
		context_.port = port;
		context_.serverName = host;

		if (!Init())			return context_;
		if (!AllocSocketSet(1)) return context_;
		if (!ResolveHost(context_.serverName.c_str())) return context_;
		if (!ResolveIp())		return context_;
		if (!OpenTcp())			return context_;
		if (!AddSocketTcp())	return context_;
		if (WaitForAcceptConnection() != ClientCreator::WAIT_FOR_AUTHENTICATION) return context_;
		if (!WaitForAuthentication(username, password)) return context_;
		isCreated = true;

		return context_;
	}
	ClientCreator::ConnectionState ClientCreator::WaitForAcceptConnection()
	{		
		sdlNetWrapper_->CheckSockets(context_.socketSet, 5000);	

		Receiver receiver;
		auto msg = receiver.Receive(context_.socket);

		if (msg == nullptr)
			return ClientCreator::NOT_CONNECTED;

		auto connectingMsg = GetAndValidateConnectionMessage(msg.get());
		if (connectingMsg == nullptr) return ClientCreator::NOT_CONNECTED;
		

		if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
		{
			Log("Joining server now...");
			return ClientCreator::CONNECTED;
		}
		
		if (connectingMsg->connectionStatus != ConnectionStatus::WAIT_FOR_AUTHENTICATION)
			return ClientCreator::NOT_CONNECTED;

		Log("Wait for authentication...");
		return ClientCreator::WAIT_FOR_AUTHENTICATION;
	}

	bool ClientCreator::WaitForAuthentication(const std::string& username, const std::string& password)
	{
		AuthenticationMessage msg(username, password);

		Sender sender;
		sender.SendTcp(context_.socket, &msg);
		
		sdlNetWrapper_->CheckSockets(context_.socketSet, 5000);		

		Receiver receiver;
		auto recvMsg = receiver.Receive(context_.socket);

		if (recvMsg == nullptr)
			return false;

		auto connectingMsg = GetAndValidateConnectionMessage(recvMsg.get());
		if (connectingMsg == nullptr) return false;

		if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
		{
			Log("Joining server now...");
			return true;
		}

		Log("Authentication failed. Wrong username or password.");
		return false;
	}

	ConnectionMessage* ClientCreator::GetAndValidateConnectionMessage(IMessage* msg)
	{
		if (msg == nullptr)
		{
			Log("[Error][ClientCreator::GetAndValidateConnectionMessage] Recv nullptr msg.");
			return nullptr;
		}

		if (msg->GetType() != MessageTypes::ConnectionMsg)
		{
			Log("[Error][ClientCreator::GetAndValidateConnectionMessage] Unsupported msg recv.");
			return nullptr;
		}

		auto connectingMsg = dynamic_cast<ConnectionMessage*>(msg);

		if (connectingMsg == nullptr)
		{
			Log("[Error][ClientCreator::GetAndValidateConnectionMessage] Something went wrong. Couldn't cast to ConnectionMessage*.");
			return nullptr;
		}

		if (connectingMsg->connectionStatus == ConnectionStatus::ERROR_FULL)
		{
			Log("Not connected. Server is full.");
			return nullptr;
		}

		return connectingMsg;
	}
} // Network