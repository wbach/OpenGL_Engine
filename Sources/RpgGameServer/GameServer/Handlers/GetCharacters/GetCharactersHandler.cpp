#include "GetCharactersHandler.h"
#include <Common/Messages/GetCharacters/GetCharactersMsgReq.h>
#include <Common/Messages/GetCharacters/GetCharactersMsgResp.h>

namespace GameServer
{
namespace Handler
{
void GetCharactersHandler::ProcessMessage(Network::UserId userId, const Network::IMessage& message)
{
    auto& user          = context_.GetUser(userId);
    auto userCharacters = user.GetCharacters();

    auto charactersMsgResp = std::make_unique<common::GetCharactersMsgResp>();

    uint8 i = 0;
    for (const auto& characterId : userCharacters)
    {
        if (i >= common::charactersPerAcount)
            break;

        auto c = context_.databaseWrapper_->GetCharacterData(characterId);

        if (!c)
        {
            DEBUG_LOG("GetCharactersHandler::ProcessMessage character not found " + std::to_string(characterId));
            continue;
        }

        auto& character = c.value();

        charactersMsgResp->characterInfo[i++] =
            common::CharacterInfo(characterId, character.name, character.lvl, character.classId);
    }

    context_.sendMessage_(userId, *charactersMsgResp);
}
}  // Handler
}  // GameServer
