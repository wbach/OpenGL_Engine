#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Hero/Stats.h"
#include "Common/Messages/MessageTypes.h"

namespace common
{
struct GetCharacterDataMsgResp : public Network::IMessage
{
    GetCharacterDataMsgResp()
        : IMessage(MessageTypes::GetCharacterDataResp, Network::MessageTarget::Dedicated)
    {
    }

    uint32 networkCharcterId;
    common::Hero::CommonStats commonStats;
    common::Hero::CharacterData characterData;
    vec3 position;
    vec3 rotation;

    virtual std::string ToString() const override
    {
        std::string result = "GetCharacterDataMsgResp{";
        return result + "\n}";
    }
};
}  // namespace common
