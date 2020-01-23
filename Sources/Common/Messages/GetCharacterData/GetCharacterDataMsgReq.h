#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct GetCharacterDataMsgReq : public Network::IMessage
{
    GetCharacterDataMsgReq()
        : IMessage(MessageTypes::GetCharacterDataReq, Network::MessageTarget::Dedicated)
    {
    }

    uint32 networkCharacterId;

    virtual std::string ToString() override
    {
        return "GetCharactersMsgReq";
    }
};
}  // namespace common
