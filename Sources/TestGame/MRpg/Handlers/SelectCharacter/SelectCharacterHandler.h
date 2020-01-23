#pragma once
#include <Common/MessageHandling/AbstractHandler.h>
#include <Common/Messages/GetCharacters/CharacterInfo.h>
#include <Common/Messages/MessageTypes.h>

namespace MmmoRpg
{
class SelectCharacterHandler : public common::AbstractHandler
{
    typedef std::function<void(uint32, uint32, bool)> Callback;

   public:
    SelectCharacterHandler(Callback callback)
        : common::AbstractHandler({common::MessageTypes::SelectCharacterResp})
        , callback_(callback)
    {
    }

   protected:
    virtual void ProcessMessage(const Network::IMessage& message) override;
    Callback callback_;
};
}  // namespace MmmoRpg
