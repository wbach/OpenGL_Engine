#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct GetCharactersMsgReq : public Network::IMessage
{
    GetCharactersMsgReq()
        : IMessage(MessageTypes::GetCharactersReq, Network::MessageTarget::Dedicated)
    {
    }

    virtual std::string ToString() override
    {
        return "GetCharactersMsgReq";
    }
};
}  // namespace Network
