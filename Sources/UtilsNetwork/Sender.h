#pragma once
#include <memory>
#include "IMessage.h"
#include "IMessageConverter.h"
#include "ISDLNetWrapper.h"
#include "Logger/Log.h"
#include "MessageFormat.h"
#include "SDLNetWrapper.h"
#include "Types.h"

namespace Network
{
enum class SentStatus
{
    OK,
    ERROR,
    EMPTY,
    CAST_ERROR
};

class Sender
{
public:
    Sender(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>&);
    SentStatus SendTcp(TCPsocket socket, const IMessage &msg);
    void SetMessageFormat(MessageFormat format);

private:
    bool sendMessageFormat(TCPsocket socket);
    bool sendMessageType(TCPsocket socket, const IMessage& msg);
    bool sendMessage(TCPsocket socket, const IMessage& msg);

    template <class T>
    SentStatus SendIMessage(TCPsocket socket, IMessage* msg)
    {
        auto final_msg = castMessageAs<T>(msg);
        if (final_msg == nullptr)
        {
            ERROR_LOG("Something went wrong. Couldn't cast to : " + std::to_string(msg->GetType()));
            return SentStatus::CAST_ERROR;
        }

        int length    = sizeof(T);
        int sentBytes = sdlNetWrapper_.SendTcp(socket, final_msg, sizeof(T));

        if (sentBytes < length)
            return SentStatus::ERROR;

        return SentStatus::OK;
    }

private:
    ISDLNetWrapper& sdlNetWrapper_;
    MessageFormat messageFormat_;
    std::vector<std::unique_ptr<IMessageConverter>>& messageConverters_;
};
}  // namespace Network
