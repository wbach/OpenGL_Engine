#include "Sender.h"

#include <Logger/Log.h>

namespace Network
{
Sender::Sender(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>& converters)
    : sdlNetWrapper_(sdlNetWrapper)
    , messageConverters_(converters)
{
}
SentStatus Sender::SendTcp(TCPsocket socket, IMessage& msg, MessageFormat format)
{
    if (not sendMessageFormat(socket, format))
        return SentStatus::ERROR;

    if (not sendMessageType(socket, msg))
        return SentStatus::ERROR;

    if (not sendMessage(socket, msg, format))
        return SentStatus::ERROR;

    return SentStatus::OK;
}

bool Sender::sendMessageFormat(TCPsocket socket, MessageFormat format)
{
    auto formatValue = Network::ConvertFormat(format);

    auto length    = sizeof(uint8);
    auto sentBytes = sdlNetWrapper_.SendTcp(socket, &formatValue, length);

    if (sentBytes < length)
        return false;

    DEBUG_LOG("formatValue sent : " + std::to_string(formatValue));
    return true;
}

bool Sender::sendMessageType(TCPsocket socket, IMessage& msg)
{
    uint8 type  = msg.GetType();
    auto length = sizeof(uint8);

    auto sentBytes = sdlNetWrapper_.SendTcp(socket, &type, length);

    if (sentBytes < length)
        return false;

    DEBUG_LOG("Msssage type sent : " + std::to_string(type));

    return true;
}

bool Sender::sendMessage(TCPsocket socket, IMessage& msg, MessageFormat format)
{
    for (auto& converter : messageConverters_)
    {
        if (converter->GetFormat() != ConvertFormat(format))
        {
            continue;
        }

        auto convertedMsg = converter->Convert(msg);

        auto length    = sizeof(uint8) * convertedMsg.size();
        auto sentBytes = sdlNetWrapper_.SendTcp(socket, &convertedMsg[0], length);

        if (sentBytes < length)
            return false;

        DEBUG_LOG("Msssage type sent : " + std::to_string(msg.GetType()));
        return true;
    }

    return false;
}
}  // namespace Network
