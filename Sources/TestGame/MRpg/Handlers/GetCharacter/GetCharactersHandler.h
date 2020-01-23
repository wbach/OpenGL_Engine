#pragma once
#include <Common/MessageHandling/AbstractHandler.h>
#include <Common/Messages/GetCharacters/CharacterInfo.h>

namespace MmmoRpg
{
class GetCharactersHandler : public common::AbstractHandler
{
    typedef std::function<void(const std::vector<common::CharacterInfo>&)> GetCharacterFunc;

   public:
    GetCharactersHandler(GetCharacterFunc characters_)
        : common::AbstractHandler({common::MessageTypes::GetCharactersResp})
        , getCharacter_(characters_)
    {
    }

   protected:
    virtual void ProcessMessage(const Network::IMessage& message) override;
    GetCharacterFunc getCharacter_;
};
}  // namespace MmmoRpg
