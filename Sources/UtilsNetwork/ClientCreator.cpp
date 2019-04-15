#include "ClientCreator.h"
#include "Utils.h"
#include "Logger/Log.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/Conntection/AuthenticationMessage.h"

namespace Network
{
	ClientCreator::ClientCreator(Sender& sender, Receiver& receiver, ISDLNetWrapperPtr sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
		, sender_(sender)
		, receiver_(receiver)
	{
	}

	wb::optional<ConectContext> ClientCreator::ConnectToServer(const std::string& username, const std::string& password, const std::string& host, uint32 port)
	{
		context_.port = port;
		context_.serverName = host;

		if (!Init())			return wb::optional<ConectContext>();
		if (!AllocSocketSet(1)) return wb::optional<ConectContext>();
		if (!ResolveHost(context_.serverName.c_str())) return wb::optional<ConectContext>();
		if (!ResolveIp())		return wb::optional<ConectContext>();
		if (!OpenTcp())			return wb::optional<ConectContext>();
		if (!AddSocketTcp())	return wb::optional<ConectContext>();
		if (WaitForAcceptConnection() != ClientCreator::WAIT_FOR_AUTHENTICATION) return wb::optional<ConectContext>();
		if (!WaitForAuthentication(username, password)) return wb::optional<ConectContext>();
		isCreated = true;

		return context_;
	}
	ClientCreator::ConnectionState ClientCreator::WaitForAcceptConnection()
	{		
		sdlNetWrapper_->CheckSockets(context_.socketSet, 5000);	
		
		RecvError err;
		auto msg = receiver_.Receive(context_.socket, err);

		if (msg == nullptr)
			return ClientCreator::NOT_CONNECTED;

		auto connectingMsg = GetAndValidateConnectionMessage(msg.get());
		if (connectingMsg == nullptr) return ClientCreator::NOT_CONNECTED;		

		if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
		{
			DEBUG_LOG("Joining server now...");
			return ClientCreator::CONNECTED;
		}
		
		if (connectingMsg->connectionStatus != ConnectionStatus::WAIT_FOR_AUTHENTICATION)
			return ClientCreator::NOT_CONNECTED;

		DEBUG_LOG("Wait for authentication...");
		return ClientCreator::WAIT_FOR_AUTHENTICATION;
	}

	bool ClientCreator::WaitForAuthentication(const std::string& username, const std::string& password)
	{
		AuthenticationMessage msg(username, password);

		sender_.SendTcp(context_.socket, &msg);
		
		sdlNetWrapper_->CheckSockets(context_.socketSet, 5000);		

		RecvError err;
		auto recvMsg = receiver_.Receive(context_.socket, err);

		if (recvMsg == nullptr)
			return false;

		auto connectingMsg = GetAndValidateConnectionMessage(recvMsg.get());
		if (connectingMsg == nullptr) return false;

		if (connectingMsg->connectionStatus == ConnectionStatus::CONNECTED)
		{
			DEBUG_LOG("Joining server now...");
			return true;
		}

		DEBUG_LOG("Authentication failed. Wrong username or password.");
		return false;
	}

	ConnectionMessage* ClientCreator::GetAndValidateConnectionMessage(IMessage* msg)
	{
		if (msg == nullptr)
		{
			ERROR_LOG("[ClientCreator::GetAndValidateConnectionMessage] Recv nullptr msg.");
			return nullptr;
		}

		if (msg->GetType() != MessageTypes::ConnectionMsg)
		{
			ERROR_LOG("[ClientCreator::GetAndValidateConnectionMessage] Unsupported msg recv.");
			return nullptr;
		}

		auto connectingMsg = dynamic_cast<ConnectionMessage*>(msg);

		if (connectingMsg == nullptr)
		{
			ERROR_LOG("[ClientCreator::GetAndValidateConnectionMessage] Something went wrong. Couldn't cast to ConnectionMessage*.");
			return nullptr;
		}

		if (connectingMsg->connectionStatus == ConnectionStatus::ERROR_FULL)
		{
			ERROR_LOG("Not connected. Server is full.");
			return nullptr;
		}

		return connectingMsg;
	}
} // Network