#pragma once
#include <memory>
#include <vector>
#include "Logger/Log.h"
#include "MessageFormat.h"
#include "Types.h"
#include <SDL2/SDL_net.h>

namespace Network
{
class ISDLNetWrapper;
class IMessageConverter;
class IMessage;

enum class SentStatus
{
    OK = 0,
    UNDEFINED_ERROR,
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
    Network::IMessageConverter* GetConverter(const IMessage&);
    bool sendMessageFormat(TCPsocket socket);
    bool sendMessageType(TCPsocket socket, const IMessage& msg);
    bool sendMessage(TCPsocket socket, const IMessage& msg, Network::IMessageConverter&);

    template <class T>
    SentStatus SendIMessage(TCPsocket socket, IMessage* msg);

private:
    ISDLNetWrapper& sdlNetWrapper_;
    MessageFormat messageFormat_;
    std::vector<std::unique_ptr<IMessageConverter>>& messageConverters_;
};
}  // namespace Network
