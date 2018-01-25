#include "Reciever.h"
#include "Logger/Log.h"
#include "GLM/GLMUtils.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/TransformMessages/TransformMsg.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "Messages/GetCharacters/GetCharactersMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"

namespace Network
{
	Receiver::Receiver(ISDLNetWrapperPtr sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
	{
	}

	std::shared_ptr<IMessage> Receiver::Receive(TCPsocket socket, RecvError& error)
	{
		error = RecvError::None;

		if (sdlNetWrapper_->SocketReady((SDLNet_GenericSocket) socket) == 0)
			return nullptr;

		Log("Receiver::Receive");
		std::shared_ptr<IMessage> result;

		//char buffer[512];
		MessageHeader header;
		int recvBytes = sdlNetWrapper_->RecvTcp(socket, &header, sizeof(header));
		Log(std::string("Receive header, msg type : ") + std::to_string(header.msgType));

		if (recvBytes == -1)
		{
			Log("Recv header bytes : -1, Disconnect.");
			error = RecvError::Disconnect;
			return nullptr;
		}

		if (recvBytes == 0)
		{
			Log("Recv header bytes: " + std::to_string(recvBytes));
			error = RecvError::ZeroBytes;
			return nullptr;
		}			

		switch (header.msgType)
		{
		case MessageTypes::ConnectionMsg:	result = GetIMessage<ConnectionMessage>(socket);		break;
		case MessageTypes::Transform:		result = GetIMessage<TransformMsg>(socket);				break;
		case MessageTypes::Authentication:	result = GetIMessage<AuthenticationMessage>(socket);	break;
		case MessageTypes::SelectCharacterReq:	result = GetIMessage<SelectCharacterMsgReq>(socket);	break;
		case MessageTypes::SelectCharacterResp:	result = GetIMessage<SelectCharacterMsgResp>(socket);	break;
		case MessageTypes::GetCharactersReq:	result = GetIMessage<GetCharactersMsgReq>(socket);	break;
		case MessageTypes::GetCharactersResp:	result = GetIMessage<GetCharactersMsgResp>(socket);	break;
		}
		return result;
	}
} // Network