#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageStatus.h"
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct SelectCharacterMsgResp : public Network::IMessage
{
    SelectCharacterMsgResp()
        : IMessage(MessageTypes::SelectCharacterResp, Network::MessageTarget::Dedicated)
    {
    }

    uint32 id    = 0;
    uint32 mapId = 0;
    MessageStatus status_;

    virtual std::string ToString() override
    {
        return "Selected character ID: " + std::to_string(id) + " on map : " + std::to_string(mapId) +
               " status : " + (status_ == MessageStatus::Ok ? "Ok" : "Fail");
    }
};
}  // namespace Network
