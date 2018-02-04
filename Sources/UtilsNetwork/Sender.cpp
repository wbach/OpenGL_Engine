#include "Sender.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "Messages/TransformMessages/TransformMsgResp.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "Messages/GetCharacters/GetCharactersMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgReq.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include "Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "Time/TimeMeasurer.h"
#include "Logger/Log.h"

namespace Network
{
	Sender::Sender(Utils::Time::CTimeMeasurer& tm, ISDLNetWrapperPtr sdlNetWrapper)
		: sdlNetWrapper_(sdlNetWrapper)
		, timer_(tm)
		, sentBytes_(0)
	{
		timer_.AddOnTickCallback(std::bind(&Sender::PrintSentBytesPerSec, this));
	}	

#define Convert(messageType, type) case messageType: if (SendIMessage<type>(socket, msg) != SentStatus::OK)	return SentStatus::ERROR; break;

	SentStatus Sender::SendTcp(TCPsocket socket, IMessage* msg)
	{
		if (msg == nullptr)
		{
			Log("Sender::SendTcp: Try send nullptr msg.");
			return SentStatus::EMPTY;
		}

		MessageHeader header;
		header.msgType = msg->GetType();

		int length = sizeof(header);
		auto sentBytes = sdlNetWrapper_->SendTcp(socket, &header, length);
		//Log("Sent header bytes : " + std::to_string(sentBytes) + "/" + std::to_string(sizeof(header)));

		if (sentBytes < length)
			return SentStatus::ERROR;		
		
		Log("Sent : " + Network::to_string(msg->GetType()));

		sentBytes_ += sentBytes;

		switch (msg->GetType())
		{
			Convert(MessageTypes::ConnectionMsg,		ConnectionMessage);
			Convert(MessageTypes::TransformReq,			TransformMsgReq);
			Convert(MessageTypes::TransformResp,		TransformMsgResp);
			Convert(MessageTypes::Authentication,		AuthenticationMessage);
			Convert(MessageTypes::SelectCharacterReq,	SelectCharacterMsgReq);
			Convert(MessageTypes::SelectCharacterResp,	SelectCharacterMsgResp);
			Convert(MessageTypes::GetCharactersReq,		GetCharactersMsgReq);
			Convert(MessageTypes::GetCharactersResp,	GetCharactersMsgResp);
			Convert(MessageTypes::GetCharacterDataReq,  GetCharacterDataMsgReq);
			Convert(MessageTypes::GetCharacterDataResp, GetCharacterDataMsgResp);
			Convert(MessageTypes::GetCharactersDataReq, GetCharactersDataMsgReq);
		}

		return SentStatus::OK;
	}
	void Sender::PrintSentBytesPerSec()
	{
		auto recvPerSec = sentBytes_;// / 1000;
		Log("Send : " + std::to_string(recvPerSec) + " B/s");
		sentBytes_ = 0;
	}

}

