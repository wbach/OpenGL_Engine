#pragma once
#include <Logger/Log.h>
#include <Types.h>

#include <memory>
#include <optional>

#include "IMessageConverter.h"
#include "ISDLNetWrapper.h"
#include "MessageFormat.h"
#include "SDLNetWrapper.h"

namespace Network
{
class IMessage;

enum class RecvStatus
{
    Ok,
    NotReady,
    Disconnect,
    UnknownConverter
};

class Receiver
{
public:
    Receiver(ISDLNetWrapper& sdlNetWrapper, std::vector<std::unique_ptr<IMessageConverter>>&);
    std::tuple<RecvStatus, std::unique_ptr<IMessage>> Receive(TCPsocket socket);

private:
    std::optional<uint8> ReceiveFormat(TCPsocket socket);
    std::optional<uint8> ReceiveType(TCPsocket socket);
    std::optional<uint32> ReceiveMessageSize(TCPsocket socket);
    std::vector<int8> ReceiveMessage(TCPsocket socket, uint32 messageSize);

private:
    ISDLNetWrapper& sdlNetWrapper_;
    std::vector<std::unique_ptr<IMessageConverter>>& messageConverters_;
};
}  // namespace Network
