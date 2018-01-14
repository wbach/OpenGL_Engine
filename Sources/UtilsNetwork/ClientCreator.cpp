#include "ClientCreator.h"
#include "Utils.h"
#include "Logger/Log.h"
#include "Reciever.h"
#include "Message.h"
#include "Sender.h"

const char fileName[] = "./server.conf";

namespace Network
{
	ClientCreator::ClientCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
	{
		serverName = Utils::ReadFile(fileName);
		Log("Server : " + serverName);
	}

	ClientCreator::ClientCreator(ISDLNetWrapper * sdlNetWrapper)
	{
		serverName = Utils::ReadFile(fileName);
		Log("Server : " + serverName);
	}

	ConectContext ClientCreator::ConnectToServer(const std::string& username, const std::string& password, uint32 port)
	{
		context_.port = port;

		if (!Init())			return context_;
		if (!AllocSocketSet(1)) return context_;
		if (!ResolveHost(serverName.c_str())) return context_;
		if (!ResolveIp())		return context_;
		if (!OpenTcp())			return context_;
		if (!AddSocketTcp())	return context_;
		if (WaitForAcceptConnection() != ClientCreator::WAIT_FOR_AUTHENTICATION) return context_;
		if (!WaitForAuthentication(username, password)) return context_;

		return context_;
	}
	ClientCreator::ConnectionState ClientCreator::WaitForAcceptConnection()
	{		
		sdlNetWrapper_->CheckSockets(context_.socketSet, 5000);
		int gotServerResponse = sdlNetWrapper_->SocketReady((SDLNet_GenericSocket) context_.socket);

		if (gotServerResponse == 0)
			return ClientCreator::NOT_CONNECTED;

		Receiver receiver;
		auto msg = receiver.Receive(context_.socket);

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
		int gotServerResponse = sdlNetWrapper_->SocketReady((SDLNet_GenericSocket)context_.socket);

		if (gotServerResponse == 0)
			return false;

		Receiver receiver;
		auto recvMsg = receiver.Receive(context_.socket);

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