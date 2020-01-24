#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct GetCharactersDataMsgReq : public Network::IMessage
{
    GetCharactersDataMsgReq()
        : IMessage(MessageTypes::GetCharactersDataReq, Network::MessageTarget::Dedicated)
    {
    }

    uint32 mapId;

    virtual std::string ToString() const override
    {
        return "GetCharactersMsgReq";
    }
};
}  // namespace Network
