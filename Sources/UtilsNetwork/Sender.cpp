#include "Sender.h"

#include "Logger/Log.h"
#include "Messages/Conntection/AuthenticationMessage.h"
#include "Messages/Conntection/ConnectionMessage.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgReq.h"
#include "Messages/GetCharacterData/GetCharacterDataMsgResp.h"
#include "Messages/GetCharacterData/GetCharactersDataMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgReq.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"
#include "Messages/Other/OtherMsg.h"
#include "Messages/RemoveCharacter/DisconnectCharacterMsg.h"
#include "Messages/SelectCharacter/SelectCharacterMsgReq.h"
#include "Messages/SelectCharacter/SelectCharacterMsgResp.h"
#include "Messages/TransformMessages/TransformMsgReq.h"
#include "Messages/TransformMessages/TransformMsgResp.h"
#include "Time/TimeMeasurer.h"
//#include <ctime>
namespace Network
{
Sender::Sender(Utils::Time::CTimeMeasurer& tm, ISDLNetWrapperPtr sdlNetWrapper)
    : sdlNetWrapper_(sdlNetWrapper)
    , timer_(tm)
    , sentBytes_(0)
{
    timer_.AddOnTickCallback(std::bind(&Sender::PrintSentBytesPerSec, this));
}

#define Convert(messageType, type)                             \
    case messageType:                                          \
        if (SendIMessage<type>(socket, msg) != SentStatus::OK) \
            return SentStatus::ERROR;                          \
        break;

SentStatus Sender::SendTcp(TCPsocket socket, IMessage* msg)
{
    DEBUG_LOG("Times test : Sent time: " + std::to_string(clock() * 1000.0f / (float)CLOCKS_PER_SEC));

    if (msg == nullptr)
    {
        DEBUG_LOG("Sender::SendTcp: Try send nullptr msg.");
        return SentStatus::EMPTY;
    }

    MessageHeader header;
    header.msgType = msg->GetType();

    int length     = sizeof(header);
    auto sentBytes = sdlNetWrapper_->SendTcp(socket, &header, length);
    // DEBUG_LOG("Sent header bytes : " + std::to_string(sentBytes) + "/" + std::to_string(sizeof(header)));

    if (sentBytes < length)
        return SentStatus::ERROR;

    DEBUG_LOG("Sent : " + Network::to_string(msg->GetType()));

    sentBytes_ += sentBytes;

    switch (msg->GetType())
    {
        Convert(MessageTypes::ConnectionMsg, ConnectionMessage);
        Convert(MessageTypes::DisconnectCharacter, DisconnectCharacterMsg);
        Convert(MessageTypes::TransformReq, TransformMsgReq);
        Convert(MessageTypes::TransformResp, TransformMsgResp);
        Convert(MessageTypes::Authentication, AuthenticationMessage);
        Convert(MessageTypes::SelectCharacterReq, SelectCharacterMsgReq);
        Convert(MessageTypes::SelectCharacterResp, SelectCharacterMsgResp);
        Convert(MessageTypes::GetCharactersReq, GetCharactersMsgReq);
        Convert(MessageTypes::GetCharactersResp, GetCharactersMsgResp);
        Convert(MessageTypes::GetCharacterDataReq, GetCharacterDataMsgReq);
        Convert(MessageTypes::GetCharacterDataResp, GetCharacterDataMsgResp);
        Convert(MessageTypes::GetCharactersDataReq, GetCharactersDataMsgReq);
        Convert(MessageTypes::Other, OtherMsg);
    }

    return SentStatus::OK;
}
void Sender::PrintSentBytesPerSec()
{
    auto recvPerSec = sentBytes_;  // / 1000;
    DEBUG_LOG("Send : " + std::to_string(recvPerSec) + " B/s");
    std::cout << " Send : " << std::to_string(recvPerSec) << " B/s" << std::endl;
    sentBytes_ = 0;
}

}  // namespace Network
