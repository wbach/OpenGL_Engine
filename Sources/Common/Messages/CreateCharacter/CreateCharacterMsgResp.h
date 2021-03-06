#pragma once
#include <UtilsNetwork/IMessage.h>
#include <UtilsNetwork/MessageTarget.h>
#include "Common/Messages/MessageStatus.h"
#include "Common/Messages/MessageTypes.h"

namespace common
{
class CreateCharacterMsgResp : public Network::IMessage
{
public:
    CreateCharacterMsgResp()
        : CreateCharacterMsgResp("")
    {
    }
    CreateCharacterMsgResp(const std::string& name)
        : IMessage(MessageTypes::CreateCharacterResp, Network::MessageTarget::Dedicated)
    {
        memset(characterName_, 0, Network::arraySize);
        Network::CopyToArray(characterName_, name);
    }
    virtual std::string ToString() const override
    {
        return "Created character ID: " + std::to_string(id) + " status : " + (status_ == MessageStatus::Ok ? "Ok" : "Fail");
    }
    std::string GetCharacterName()
    {
        return characterName_;
    }

public:
    uint32 id = 0;
    MessageStatus status_;

private:
    char characterName_[Network::arraySize];
};
}  // namespace common
