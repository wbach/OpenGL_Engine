#include "GetCharactersHandler.h"
#include <Common/Messages/GetCharacters/GetCharactersMsgResp.h>

namespace MmmoRpg
{
void GetCharactersHandler::ProcessMessage(Network::UserId, const Network::IMessage& message)
{
    auto msg = static_cast<const common::GetCharactersMsgResp*>(&message);

    if (msg == nullptr)
    {
        DEBUG_LOG("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " + std::to_string(message.GetType()));
        return;
    }

    std::vector<common::CharacterInfo> characters;
    for (const auto& cd : msg->characterInfo)
    {
        if (not cd)
        {
            continue;
        }
        characters.push_back(*cd);
    }
    getCharacter_(characters);
}

}  // namespace MmmoRpg
