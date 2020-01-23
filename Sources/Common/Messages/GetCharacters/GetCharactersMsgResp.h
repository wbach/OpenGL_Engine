#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"
#include "CharacterInfo.h"
#include <optional>

namespace common
{
const uint8 charactersPerAcount = 5;

struct GetCharactersMsgResp : public Network::IMessage
{
    GetCharactersMsgResp()
        : IMessage(MessageTypes::GetCharactersResp, Network::MessageTarget::Dedicated)
    {
    }

    std::optional<CharacterInfo> characterInfo[charactersPerAcount];

    virtual std::string ToString() override
    {
        std::string result = "GetCharactersMsgResp characters: {";

        for (const auto& ci : characterInfo)
        {
            if (not ci)
            {
                continue;
            }

            result += "\nId : " + std::to_string(ci->id_);
        }

        return result + "\n}";
    }
};
}  // namespace Network
