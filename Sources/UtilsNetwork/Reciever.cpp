#include "Reciever.h"
#include "Logger/Log.h"
#include "GLM/GLMUtils.h"
#include "Message.h"

namespace Network
{
	Receiver::Receiver(ISDLNetWrapper* sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
	{
	}

	std::shared_ptr<IMessage> Receiver::Receive(TCPsocket socket)
	{
		if (sdlNetWrapper_->SocketReady((SDLNet_GenericSocket) socket) == 0)
			return nullptr;

		Log("Receiver::Receive");
		std::shared_ptr<IMessage> result;

		//char buffer[512];
		MessageHeader header;
		int recvBytes = sdlNetWrapper_->RecvTcp(socket, &header, sizeof(header));
		Log(std::string("Receive header, msg type : ") + std::to_string(header.msgType));

		if (recvBytes <= 0)
			return nullptr;

		switch (header.msgType)
		{
		case MessageTypes::ConnectionMsg:	result = GetIMessage<ConnectionMessage>(socket);		break;
		case MessageTypes::TestDataMsg:		result = GetIMessage<TransformMsg>(socket);					break;
		case MessageTypes::Authentication:	result = GetIMessage<AuthenticationMessage>(socket);	break;
		}
		return result;
	}
} // Network