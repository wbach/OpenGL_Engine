#include "Sender.h"
#include "Logger/Log.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/TransformMessages/TransformMsg.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"

namespace Network
{
	Sender::Sender(ISDLNetWrapper * sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
	{
	}	

	bool Sender::SendTcp(TCPsocket socket, IMessage* msg)
	{
		if (msg == nullptr)
		{
			Log("Sender::SendTcp: Try send nullptr msg.");
			return false;
		}

		MessageHeader header;
		header.msgType = msg->GetType();
		
		auto sentBytes = sdlNetWrapper_->SendTcp(socket, &header, sizeof(header));
		Log("Sent header bytes : " + std::to_string(sentBytes) + "/" + std::to_string(sizeof(header)));

		if (sentBytes == 0)
			return false;

		switch (msg->GetType())
		{
		case MessageTypes::ConnectionMsg:	if (!SendIMessage<ConnectionMessage>(socket, msg))		return false;	break;
		case MessageTypes::Transform: 	if (!SendIMessage<TransformMsg>(socket, msg))				return false;	break;
		case MessageTypes::Authentication: 	if (!SendIMessage<AuthenticationMessage>(socket, msg))	return false;	break;
		case MessageTypes::SelectCharacterReq: 	if (!SendIMessage<SelectCharacterMsgReq>(socket, msg))		return false;	break;
		case MessageTypes::SelectCharacterResp: 	if (!SendIMessage<SelectCharacterMsgResp>(socket, msg))	return false;	break;

		}

		return true;
	}	
}

