#include "Sender.h"
#include <Logger/Log.h>

namespace Network
{
Sender::Sender(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>& converters)
    : sdlNetWrapper_(sdlNetWrapper)
    , messageConverters_(converters)
{
}

#define Convert(messageType, type)                             \
    case messageType:                                          \
        if (SendIMessage<type>(socket, msg) != SentStatus::OK) \
            return SentStatus::ERROR;                          \
        break;

SentStatus Sender::SendTcp(TCPsocket socket, IMessage& msg, MessageFormat format)
{
    {
        auto formatValue = Network::ConvertFormat(format);

        auto length    = sizeof(uint8);
        auto sentBytes = sdlNetWrapper_.SendTcp(socket, &formatValue, length);

        if (sentBytes < length)
            return SentStatus::ERROR;

        DEBUG_LOG("formatValue Sent : " + std::to_string(formatValue));
    }

    {
        uint8 type  = msg.GetType();
        auto length = sizeof(uint8);

        auto sentBytes = sdlNetWrapper_.SendTcp(socket, &type, length);

        if (sentBytes < length)
            return SentStatus::ERROR;

        DEBUG_LOG("MEssage Type Sent : " + std::to_string(type));
    }

    switch (msg.GetType())
    {
        //        Convert(MessageTypes::ConnectionMsg, ConnectionMessage);
        //        Convert(MessageTypes::DisconnectCharacter, DisconnectCharacterMsg);
        //        Convert(MessageTypes::TransformReq, TransformMsgReq);
        //        Convert(MessageTypes::TransformResp, TransformMsgResp);
        //        Convert(MessageTypes::Authentication, AuthenticationMessage);
        //        Convert(MessageTypes::SelectCharacterReq, SelectCharacterMsgReq);
        //        Convert(MessageTypes::SelectCharacterResp, SelectCharacterMsgResp);
        //        Convert(MessageTypes::GetCharactersReq, GetCharactersMsgReq);
        //        Convert(MessageTypes::GetCharactersResp, GetCharactersMsgResp);
        //        Convert(MessageTypes::GetCharacterDataReq, GetCharacterDataMsgReq);
        //        Convert(MessageTypes::GetCharacterDataResp, GetCharacterDataMsgResp);
        //        Convert(MessageTypes::GetCharactersDataReq, GetCharactersDataMsgReq);
        //        Convert(MessageTypes::Other, OtherMsg);
    }

    return SentStatus::OK;
}

}  // namespace Network
