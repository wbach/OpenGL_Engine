#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageTypes.h"

namespace common
{
class CreateCharacterMsgReq : public Network::IMessage
{
public:
    CreateCharacterMsgReq()
        : CreateCharacterMsgReq("")
    {
    }

    CreateCharacterMsgReq(const std::string& name)
        : IMessage(MessageTypes::CreateCharacterReq, Network::MessageTarget::Dedicated)
    {
        memset(characterName_, 0, Network::arraySize);
        Network::CopyToArray(characterName_, name);
    }
    virtual std::string ToString() const override
    {
        return "Selected character ID: " + std::to_string(id);
    }
    std::string GetCharacterName()
    {
        return characterName_;
    }

public:
    uint32 id = 0;

private:
    char characterName_[Network::arraySize];
};
}  // namespace common
