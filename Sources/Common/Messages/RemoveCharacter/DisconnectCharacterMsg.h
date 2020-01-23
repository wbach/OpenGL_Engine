#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct DisconnectCharacterMsg : public Network::IMessage
{
    DisconnectCharacterMsg()
        : DisconnectCharacterMsg(0)
    {
    }
    DisconnectCharacterMsg(uint32 id)
        : Network::IMessage(MessageTypes::DisconnectCharacter, Network::MessageTarget::Dedicated)
        , id(id)
    {
    }

    uint32 id;

    virtual std::string ToString() override
    {
        return "Disconnect : " + std::to_string(id);
    }
};
}  // namespace Network
