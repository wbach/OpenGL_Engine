#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct SelectCharacterMsgReq : public Network::IMessage
{
    SelectCharacterMsgReq()
        : IMessage(MessageTypes::SelectCharacterReq, Network::MessageTarget::Dedicated)
    {
    }

    uint32 id = 0;

    virtual std::string ToString() override
    {
        return "Selected character ID: " + std::to_string(id);
    }
};
}  // namespace Network
