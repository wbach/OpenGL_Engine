#include "GetCharactersHandler.h"
#include "Messages/GetCharacters/GetCharactersMsgResp.h"

namespace MmmoRpg
{
void GetCharactersHandler::ProcessMessage(const Network::BoxMessage& message)
{
    auto msg = Network::castMessageAs<Network::GetCharactersMsgResp>(message.second.get());

    if (msg == nullptr)
    {
        DEBUG_LOG("SelectCharacterScene::WaitForGetCharacterResp, got msg but wrong type : " +
                  std::to_string(msg->GetType()));
        return;
    }

    std::vector<Network::CharacterInfo> characters;
    for (const auto& cd : msg->characterInfo)
    {
        if (!cd)
            continue;
        characters.push_back(cd.constValue());
    }
    getCharacter_(characters);
}

}  // namespace MmmoRpg
