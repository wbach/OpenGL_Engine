#include "Sender.h"

#include <Logger/Log.h>

namespace Network
{
Sender::Sender(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>& converters)
    : sdlNetWrapper_(sdlNetWrapper)
    , messageFormat_(MessageFormat::Binary)
    , messageConverters_(converters)
{
}

void Sender::SetMessageFormat(MessageFormat format)
{
    messageFormat_ = format;
}

IMessageConverter* Sender::GetConverter(const IMessage& msg)
{
    auto converter = std::find_if(messageConverters_.begin(), messageConverters_.end(), [&](auto& converter) { return converter->IsValid(static_cast<uint8>(messageFormat_), msg.GetType()); });

    return converter != messageConverters_.end() ? converter->get() : nullptr;
}
SentStatus Sender::SendTcp(TCPsocket socket, const IMessage& msg)
{
    auto converter = GetConverter(msg);

    if (not converter)
    {
        DEBUG_LOG("Converter for message not found.");
        return SentStatus::ERROR;
    }

    if (not sendMessageFormat(socket))
        return SentStatus::ERROR;

    if (not sendMessageType(socket, msg))
        return SentStatus::ERROR;

    if (not sendMessage(socket, msg, *converter))
        return SentStatus::ERROR;

    return SentStatus::OK;
}

bool Sender::sendMessageFormat(TCPsocket socket)
{
    auto formatValue = Network::ConvertFormat(messageFormat_);

    int length    = sizeof(uint8);
    auto sentBytes = sdlNetWrapper_.SendTcp(socket, &formatValue, length);

    if (sentBytes < length)
        return false;

    DEBUG_LOG("formatValue sent : " + std::to_string(formatValue));
    return true;
}

bool Sender::sendMessageType(TCPsocket socket, const IMessage& msg)
{
    uint8 type  = msg.GetType();
    int length = sizeof(uint8);

    auto sentBytes = sdlNetWrapper_.SendTcp(socket, &type, length);

    if (sentBytes < length)
        return false;

    DEBUG_LOG("Msssage type sent : " + std::to_string(type));

    return true;
}

bool Sender::sendMessage(TCPsocket socket, const IMessage& msg, IMessageConverter& converter)
{
    auto convertedMsg = converter.Convert(msg);
    auto length       = static_cast<int>(sizeof(uint8) * convertedMsg.size());
    return sdlNetWrapper_.SendTcp(socket, &convertedMsg[0], length);
}
}  // namespace Network
