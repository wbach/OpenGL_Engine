#pragma once
#include "UtilsNetwork/IMessage.h"
#include "UtilsNetwork/MessageTarget.h"
#include "UtilsNetwork/Messages/MessageTypes.h"
#include "ConnectionStatus.h"

namespace Network
{
struct ConnectionMessage : public IMessage
{
    ConnectionMessage()
        : IMessage(MessageTypes::ConnectionMsg, MessageTarget::Dedicated)
    {
    }
    ConnectionMessage(ConnectionStatus status)
        : IMessage(MessageTypes::ConnectionMsg, MessageTarget::Dedicated)
        , connectionStatus(static_cast<uint8>(status))
    {
    }
    uint8 connectionStatus = 0;

    virtual std::string ToString() override
    {
        return "ConnectionStatus: " + std::to_string(connectionStatus);
    }
};
}  // namespace Network
