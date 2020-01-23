#include "Reciever.h"
#include "MessageFormat.h"
#include <GLM/GLMUtils.h>

#define Case(x, y)                              \
    case x:                                     \
        result = GetIMessage<y>(socket, error); \
        break

namespace Network
{
Receiver::Receiver(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>& converters)
    : sdlNetWrapper_(sdlNetWrapper)
    , messageConverters_(converters)
{
}

std::tuple<RecvStatus, std::unique_ptr<IMessage>> Receiver::Receive(TCPsocket socket)
{
    if (sdlNetWrapper_.SocketReady((SDLNet_GenericSocket)socket) == 0)
        return std::make_tuple(RecvStatus::NotReady, nullptr);

    auto format = ReceiveFormat(socket);

    if (not format)
    {
        DEBUG_LOG("Recevie unknown message incoming format");
        return std::make_tuple(RecvStatus::Disconnect, nullptr);
    }
    DEBUG_LOG("Recevie message incoming format :" + std::to_string(*format));

    auto type = ReceiveType(socket);

    if (not type)
    {
        DEBUG_LOG("Recevie unknown message incoming type");
        return std::make_tuple(RecvStatus::Disconnect, nullptr);
    }
    DEBUG_LOG("Recevie message incoming type :" + std::to_string(*type));

    auto message = ReceiveMessage(socket);

    if (message.empty())
    {
        DEBUG_LOG("Recevie incoming unknown message");
        return std::make_tuple(RecvStatus::Disconnect, nullptr);
    }
    DEBUG_LOG("Recevie incoming message :" + std::to_string(*type));

    for (auto& messageConverter : messageConverters_)
    {
        if (not messageConverter->IsValid(*format, *type))
        {
            continue;
        }

        auto imessage = messageConverter->Convert(*type, message);

        if (imessage)
            return std::make_tuple(RecvStatus::Ok, std::move(imessage));
    }

    return std::make_tuple(RecvStatus::UnknownConverter, nullptr);
}

std::optional<uint8> Receiver::ReceiveFormat(TCPsocket socket)
{
    uint8 messageFormat = 0;

    if (sdlNetWrapper_.RecvTcp(socket, &messageFormat, sizeof(uint8)))
    {
        DEBUG_LOG(std::string("Receive header, msg type : ") + std::to_string(ConvertFormat(messageFormat)));
    }

    return messageFormat;
}

std::optional<uint8> Receiver::ReceiveType(TCPsocket socket)
{
    uint8 messageType = 0;
    if (sdlNetWrapper_.RecvTcp(socket, &messageType, sizeof(uint8)))
    {
        DEBUG_LOG(std::string("Receive header, msg type : ") + std::to_string(messageType));
        return messageType;
    }
    return {};
}

std::vector<int8> Receiver::ReceiveMessage(TCPsocket socket)
{
    std::vector<int8> result;

    const int BUFF_SIZE = 512;
    char buffer[BUFF_SIZE];
    memset(&buffer, 0, BUFF_SIZE);

    while (true)
    {
        if (sdlNetWrapper_.RecvTcp(socket, &buffer, BUFF_SIZE))
        {
            for (char c : buffer)
            {
                if (c == ';')
                {
                    return result;
                }

                result.push_back(c);
            }
        }
        else
        {
            return {};
        }
    }

    return result;
}
}  // namespace Network
