#include "Sender.h"

#include <Logger/Log.h>

namespace Network
{
Sender::Sender(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>& converters)
    : sdlNetWrapper_(sdlNetWrapper)
    , messageFormat_(MessageFormat::Xml)
    , messageConverters_(converters)
{
}

void Sender::SetMessageFormat(MessageFormat format)
{
    messageFormat_ = format;
}
SentStatus Sender::SendTcp(TCPsocket socket, IMessage& msg)
{
    if (not sendMessageFormat(socket))
        return SentStatus::ERROR;

    if (not sendMessageType(socket, msg))
        return SentStatus::ERROR;

    if (not sendMessage(socket, msg))
        return SentStatus::ERROR;

    return SentStatus::OK;
}

bool Sender::sendMessageFormat(TCPsocket socket)
{
    auto formatValue = Network::ConvertFormat(messageFormat_);

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

bool Sender::sendMessage(TCPsocket socket, IMessage& msg)
{
    for (auto& converter : messageConverters_)
    {
        if (not converter->IsValid(static_cast<uint8>(messageFormat_), msg.GetType()))
        {
            continue;
        }

        auto convertedMsg = converter->Convert(msg);

        auto length = sizeof(uint8) * convertedMsg.size();
        auto isSent = sdlNetWrapper_.SendTcp(socket, &convertedMsg[0], length);

        if (not isSent)
        {
            DEBUG_LOG("Send error.");
            return false;
        }

        DEBUG_LOG("Msssage type sent : " + std::to_string(msg.GetType()));
        return true;
    }

    return false;
}
}  // namespace Network
