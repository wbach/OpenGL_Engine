#pragma once
#include "NetworkCreator.h"

namespace Network
{
	struct ConnectionMessage;
	class IMessage;

	class ClientCreator : public CNetworkCreator
	{
		enum ConnectionState
		{
			NOT_CONNECTED,
			CONNECTED,
			WAIT_FOR_AUTHENTICATION
		};

	public:
		ClientCreator(std::shared_ptr<ISDLNetWrapper> sdlNetWrapper = std::shared_ptr<ISDLNetWrapper>(new SDLNetWrapper()));
		ClientCreator(ISDLNetWrapper* sdlNetWrapper);
		ConectContext ConnectToServer(const std::string& username, const std::string& password, uint32 port);

	private:
		ConnectionState WaitForAcceptConnection();
		ConnectionMessage* GetAndValidateConnectionMessage(IMessage* msg);
		bool WaitForAuthentication(const std::string& username, const std::string& password);

	private:
		std::string serverName;
		std::shared_ptr<ISDLNetWrapper> sdlNetWrapper_;
	};
}